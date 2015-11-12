#ifndef IMMOVABLE_OBJECT_HPP
#define IMMOVABLE_OBJECT_HPP

#include <string>

#include "src/robocar.pb.h"

#include <osmium/osm/types.hpp>

namespace justine
{

namespace robocar
{

class ImmovableObject
{
public:
  ImmovableObject(osmium::unsigned_object_id_type node, int id);

  virtual void assign(ImmovableObjectData *objData)
  {
    objData->set_id(id);
    objData->set_node(node);
  }

  osmium::unsigned_object_id_type getNode();

  int getId();
protected:
  osmium::unsigned_object_id_type node;
  int id;
};

class BusStop : public ImmovableObject
{
public:
  BusStop(osmium::unsigned_object_id_type node, int id, std::string name);

  virtual void assign(ImmovableObjectData *objData)
  {
    objData->set_id(id);
    objData->set_node(node);

    objData->set_name(name);
  }

  std::string getName() const
  {
    return name;
  }
protected:
  std::string name;
};

}
} // justine::robocar::


#endif
