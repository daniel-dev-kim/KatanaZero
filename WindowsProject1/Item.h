#pragma once
#include "Object.h"

class Item : public Object
{
    int thrownItemRotate = 0;
    ItemType type;
    ItemStatus status;
    std::list<Afterimage> afterimages;

public:
    double markY;
    int timer;
    Item(ObjectManager*, ObjectType);
    ~Item() {
        std::cout << "itemGone";
    }

    virtual void Update(double dt);
    virtual void Render();
    void SetStatus(ItemStatus s);
    ItemStatus GetStatus() const; 
    void SetItemType(ItemType t);
    ItemType GetItemType() const;
};

