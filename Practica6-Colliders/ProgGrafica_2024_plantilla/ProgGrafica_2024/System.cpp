#include "System.h"

vector<Object3D*> System::getCollisions(Object3D* obj, objTypes type)
{
    vector<Object3D*> res;

    for (auto& o : render->objectList)
    {
        if (obj != o && o->type == type && o->coll && obj->coll &&
            obj->coll->collisionTest(o->coll))
            res.push_back(o);
    }

    return res;
}