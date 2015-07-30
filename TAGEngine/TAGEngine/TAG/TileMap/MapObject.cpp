#include "MapObject.hpp"
#include "MapLayer.hpp"
#include "Logger.hpp"
#include "Trigonometry.hpp"

bool MapObject::Segment::intersects(const MapObject::Segment& segment)
{
    sf::Vector2f s1 = end - start;
    sf::Vector2f s2 = segment.end - segment.start;
    
    float s, t;
    s = (-s1.y * (start.x - segment.start.x) + s1.x * (start.y - segment.start.y)) / (-s2.x * s1.y + s1.x * s2.y);
    t = ( s2.x * (start.y - segment.start.y) - s2.y * (start.x - segment.start.x)) / (-s2.x * s1.y + s1.x * s2.y);
    
    if(s >= 0 && s <= 1 && t >= 0 && t <= 1)
    {
        // Collision detected
        // point at which lines intersect - do what you will with this.
        //sf::Vector2f intersection(start.x + (t * s1.x), start.y + (t * s1.y));
        return true;
    }
    return false;
}

MapObject::MapObject()
 : mVisible(true)
 , mShape(Rectangle)
 , mTileQuad(nullptr)
 , mFurthestPoint(0.f)
{}

std::string MapObject::getPropertyString(const std::string& name)
{
    if(mProperties.find(name) != mProperties.end())
    {
        return mProperties[name];
    }
    else
    {
        return std::string();
    }
}

void MapObject::setProperty(const std::string& name, const std::string& value)
{
    mProperties[name] = value;
}

void MapObject::setPosition(float x, float y)
{
    setPosition(sf::Vector2f(x, y));
}

void MapObject::setPosition(const sf::Vector2f& position)
{
    sf::Vector2f distance = position - mPosition;
    move(distance);
}

void MapObject::move(float x, float y)
{
    move(sf::Vector2f(x, y));
}

void MapObject::move(const sf::Vector2f& distance)
{
    // update properties by movement amount
    mCentrePoint += distance;
    for(auto& p : mPolypoints)
    {
        p += distance;
    }
    
    mDebugShape.move(distance);
    
    mAABB.left += distance.x;
    mAABB.top += distance.y;
    
    // set new position
    mPosition += distance;
    
    // if object is of type tile move vertex data
    if(mTileQuad) mTileQuad->move(distance);
}

bool MapObject::contains(sf::Vector2f point) const
{
    if(mShape == Polyline) return false;
    
    // convert point to local coords
    point -= mPosition;
    
    // TODO transform point instead.
    
    // check if enough poly points
    unsigned int points = mPolypoints.size();
    if(points < 3) return false;
    
    // else raycast through points.
    unsigned int i, j;
    bool result = false;
    for(i = 0, j = points - 1; i < points; j = i++)
    {
        if(((mPolypoints[i].y > point.y) != (mPolypoints[j].y > point.y)) &&
           (point.x < (mPolypoints[j].x - mPolypoints[i].x) * (point.y - mPolypoints[i].y)
            / (mPolypoints[j].y - mPolypoints[i].y) + mPolypoints[i].x))
            result = !result;
    }
    
    return result;
}

bool MapObject::intersects(const MapObject& object) const
{
    // check if distance between objects is less than sum of furthest points
    float distance = tag::length(mCentrePoint + object.mCentrePoint);
    if(distance > (mFurthestPoint + object.mFurthestPoint)) return false;
    
    // check intersection if either object contains a point of the other
    for(auto& p : object.mPolypoints)
    {
        if(contains(p + object.getPosition())) return true;
    }
    
    for(auto& p : mPolypoints)
    {
        if(object.contains(p + getPosition())) return true;
    }
    
    return false;
}

void MapObject::createDebugShape(const sf::Color& color)
{
    if(mPolypoints.size() == 0)
    {
        LOG_ERR("Unable to create debug shape for <" + name + ">, object data missing.");
        return;
    }
    
    // reset any existing shapes incase new points have been added.
    mDebugShape.reset();
    
    for(const auto& p : mPolypoints)
    {
        mDebugShape.addVertex(sf::Vertex(p, color));
    }
    
    if(mShape != Polyline) mDebugShape.closeShape();
    
    // precompute shape values for intersection testing
    calcTestValues();
    
    // create the AABB for quad tree testing
    createAABB();
}

void MapObject::drawDebugShape(sf::RenderTarget& rt) const
{
    rt.draw(mDebugShape);
}

sf::Vector2f MapObject::firstPoint() const
{
    if(!mPolypoints.empty())
    {
        return mPolypoints[0] + mPosition;
    }
    else
    {
        return sf::Vector2f();
    }
}

sf::Vector2f MapObject::lastPoint() const
{
    if(!mPolypoints.empty())
    {
        return (mPolypoints.back() + mPosition);
    }
    else
    {
         return sf::Vector2f();
    }
}

sf::Vector2f MapObject::collisionNormal(const sf::Vector2f& start, const sf::Vector2f& end) const
{
    Segment trajectory(start, end);
    sf::Vector2f dv = end - start;
    
    for(auto& s : mPolySegs)
    {
        if(trajectory.intersects(s))
        {
            sf::Vector2f v = s.end - s.start;
            sf::Vector2f n(v.y, -v.x);
            // invert normal if pointing in wrong direction
            float tAngle = tag::TrigonometricTraits<float>::arcTan2(dv.y, dv.x);
            float nAngle = tag::TrigonometricTraits<float>::arcTan2(n.y, n.x);
            if(nAngle - tAngle > 90.f) n = -n;
            
            return tag::unitVector(n);
        }
        
        sf::Vector2f rv(end - start);
        return tag::unitVector(rv);
    }
}

void MapObject::createSegments()
{
    if(mPolypoints.size() == 0)
    {
        LOG_ERR("Unable to create segments for object <" + mName + ">, object data missing.");
        return;
    }
    
    for(unsigned int i = 0u, len = mPolypoints.size()-1; i < len;  i++)
    {
        mPolySegs.push_back(Segment(mPolypoints[i], mPolypoints[i + 1]));
    }
    mPolySegs.push_back(Segment(*(mPolypoints.end() - 1), *mPolypoints.begin()));
    
    LOG_INF("Added " + std::to_string(mPolySegs.size()) + " segments to Map Object <" + mName +">");
}

bool MapObject::convex() const
{
    if(mShape == MapObjectShape::Polyline)
    {
        return false;
    }
    
    bool negative = false;
    bool positive = false;
    
    sf::Uint16 a, b, c, n = mPolypoints.size();
    for(a = 0u; a < n; ++a)
    {
        b = (a + 1) % n;
        c = (b + 1) % n;
        
        float cross = tag::crossProduct(mPolypoints[a], mPolypoints[b], mPolypoints[c]);
        
        if(cross < 0.f)
        {
            negative = true;
        }
        else if(cross > 0.f)
        {
            positive = true;
        }
        if(positive && negative)
        {
            return false;
        }
    }
    return true;
}

const std::vector<sf::Vector2f>& MapObject::polyPoints() const
{
    return mPolypoints;
}

void MapObject::reverseWinding()
{
    std::reverse(mPolypoints.begin(), mPolypoints.end());
}

void MapObject::setQuad(TileQuad* quad)
{
    mTileQuad = quad;
}

sf::Vector2f MapObject::calcCentre() const
{
    if(mShape == Rectangle || mPolypoints.size() < 3)
    {
        // we don't have a triangle so use a bounding box
        return sf::Vector2f(mPosition.x + (mSize.x / 2.f), mPosition.y + (mSize.y / 2.f));
    }
    
    // calc centroid of poly shape
    sf::Vector2f centroid;
    float signedArea = 0.f;
    float x0 = 0.f; // Current vertex X
    float y0 = 0.f; // Current vertex Y
    float x1 = 0.f; // Next vertex X
    float y1 = 0.f; // Next vertex Y
    float a = 0.f; // partial signed area

    // For all vertices except last.
    unsigned i, len = mPolypoints.size() - 1;
    for(i = 0; i < len; ++i)    
    {
        x0 = mPolypoints[i].x;
        y0 = mPolypoints[i].y;
        x1 = mPolypoints[i + 1].x;
        y1 = mPolypoints[i + 1].y;
        a = x0 * y1 - x1 * y0;
        signedArea += a;
        centroid.x += (x0 + x1) * a;
        centroid.y += (y0 + y1) * a;
    }
    
    // Do last vertex
    x0 = mPolypoints[i].x;
    y0 = mPolypoints[i].y;
    x1 = mPolypoints[0].x;
    y1 = mPolypoints[0].y;
    a = x0 * y1 - x1 * y0;
    signedArea += a;
    centroid.x += (x0 + x1) * a;
    centroid.y += (y0 + y1) * a;
    
    signedArea *= 0.5;
    centroid.x /= (6 * signedArea);
    centroid.y /= (6 * signedArea);
    
    // convert to world space
    centroid += mPosition;
    return centroid;
}

void MapObject::calcTestValues()
{
    mCentrePoint = calcCentre();
    for(auto i = mPolypoints.cbegin(); i != mPolypoints.cend(); ++i)
    {
        float length = tag::length(*i - mCentrePoint);
        if(mFurthestPoint < length)
        {
            mFurthestPoint = length;
            if(mShape == Polyline) mCentrePoint = *i / 2.f;
        }
    }
    // polyline centre ought to be half way between the start point and the furthest vertex
    if(mShape == Polyline) mFurthestPoint /= 2.f;
}

void MapObject::createAABB()
{
    if(!mPolypoints.empty())
    {
        mAABB = sf::FloatRect(mPolypoints[0], mPolypoints[0]);
        for(auto point = mPolypoints.cbegin(); point != mPolypoints.cend(); ++point)
        {
            if(point->x < mAABB.left) mAABB.left = point->x;
            if(point->x > mAABB.width) mAABB.width = point->x;
            if(point->y < mAABB.top) mAABB.top = point->y;
            if(point->y > mAABB.height) mAABB.height = point->y;
        }
        
        // calc true width and height by distance between points
        mAABB.width -= mAABB.left;
        mAABB.height -= mAABB.top;
        
        // offset into world position
        mAABB.left += mPosition.x;
        mAABB.top += mPosition.y;
        
        // debug draw AABB
        //mDebugShape.append(sf::Vector2f(mAABB.left, mAABB.top));
        //mDebugShape.append(sf::Vector2f(mAABB.left + mAABB.width, mAABB.top));
        //mDebugShape.append(sf::Vector2f(mAABB.left + mAABB.width, mAABB.top + mAABB.height));
        //mDebugShape.append(sf::Vector2f(mAABB.left, mAABB.top + mAABB.height));
    }
}