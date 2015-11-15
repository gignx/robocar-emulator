namespace justine
{

namespace robocar
{

#include <osmium/osm/location.hpp>
#include <string>
#include <vector>

struct OSMBusStop
{
  osmium::Location location;
  std::string name;
};

struct OSMBusWay
{
  std::string ref;

  // hányszor bukkantunk rá az XML-ben
  int timesFound;

  // kezdetben a Way-eket toljuk bele, utána meg feloldjuk
  // Nagyállomás -> Doberdó
  std::vector<unsigned int> nodesTo;
  // Doberdó -> Nagyállomás
  std::vector<unsigned int> nodesFrom;

  OSMBusWay(std::string r):
    ref(r), timesFound(1)
  {}
};

}
}
