#include "immovableObject.hpp"

justine::robocar::ImmovableObject::ImmovableObject(osmium::unsigned_object_id_type node, int id):
  node(node), id(id)
{}

osmium::unsigned_object_id_type justine::robocar::ImmovableObject::getNode()
{
  return node;
}

int justine::robocar::ImmovableObject::getId()
{
  return id;
}

justine::robocar::BusStop::BusStop(osmium::unsigned_object_id_type node, int id, std::string name):
  ImmovableObject(node, id), name(name)
{}
