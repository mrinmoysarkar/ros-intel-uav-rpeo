// Generated by gencpp from file vicon_bridge/viconGrabPoseRequest.msg
// DO NOT EDIT!


#ifndef VICON_BRIDGE_MESSAGE_VICONGRABPOSEREQUEST_H
#define VICON_BRIDGE_MESSAGE_VICONGRABPOSEREQUEST_H


#include <string>
#include <vector>
#include <map>

#include <ros/types.h>
#include <ros/serialization.h>
#include <ros/builtin_message_traits.h>
#include <ros/message_operations.h>


namespace vicon_bridge
{
template <class ContainerAllocator>
struct viconGrabPoseRequest_
{
  typedef viconGrabPoseRequest_<ContainerAllocator> Type;

  viconGrabPoseRequest_()
    : subject_name()
    , segment_name()
    , n_measurements(0)  {
    }
  viconGrabPoseRequest_(const ContainerAllocator& _alloc)
    : subject_name(_alloc)
    , segment_name(_alloc)
    , n_measurements(0)  {
  (void)_alloc;
    }



   typedef std::basic_string<char, std::char_traits<char>, typename ContainerAllocator::template rebind<char>::other >  _subject_name_type;
  _subject_name_type subject_name;

   typedef std::basic_string<char, std::char_traits<char>, typename ContainerAllocator::template rebind<char>::other >  _segment_name_type;
  _segment_name_type segment_name;

   typedef int32_t _n_measurements_type;
  _n_measurements_type n_measurements;





  typedef boost::shared_ptr< ::vicon_bridge::viconGrabPoseRequest_<ContainerAllocator> > Ptr;
  typedef boost::shared_ptr< ::vicon_bridge::viconGrabPoseRequest_<ContainerAllocator> const> ConstPtr;

}; // struct viconGrabPoseRequest_

typedef ::vicon_bridge::viconGrabPoseRequest_<std::allocator<void> > viconGrabPoseRequest;

typedef boost::shared_ptr< ::vicon_bridge::viconGrabPoseRequest > viconGrabPoseRequestPtr;
typedef boost::shared_ptr< ::vicon_bridge::viconGrabPoseRequest const> viconGrabPoseRequestConstPtr;

// constants requiring out of line definition



template<typename ContainerAllocator>
std::ostream& operator<<(std::ostream& s, const ::vicon_bridge::viconGrabPoseRequest_<ContainerAllocator> & v)
{
ros::message_operations::Printer< ::vicon_bridge::viconGrabPoseRequest_<ContainerAllocator> >::stream(s, "", v);
return s;
}

} // namespace vicon_bridge

namespace ros
{
namespace message_traits
{



// BOOLTRAITS {'IsFixedSize': False, 'IsMessage': True, 'HasHeader': False}
// {'std_msgs': ['/opt/ros/kinetic/share/std_msgs/cmake/../msg'], 'vicon_bridge': ['/home/intel1/ros_repo/ros_ws/src/vicon_bridge/msg'], 'geometry_msgs': ['/opt/ros/kinetic/share/geometry_msgs/cmake/../msg']}

// !!!!!!!!!!! ['__class__', '__delattr__', '__dict__', '__doc__', '__eq__', '__format__', '__getattribute__', '__hash__', '__init__', '__module__', '__ne__', '__new__', '__reduce__', '__reduce_ex__', '__repr__', '__setattr__', '__sizeof__', '__str__', '__subclasshook__', '__weakref__', '_parsed_fields', 'constants', 'fields', 'full_name', 'has_header', 'header_present', 'names', 'package', 'parsed_fields', 'short_name', 'text', 'types']




template <class ContainerAllocator>
struct IsFixedSize< ::vicon_bridge::viconGrabPoseRequest_<ContainerAllocator> >
  : FalseType
  { };

template <class ContainerAllocator>
struct IsFixedSize< ::vicon_bridge::viconGrabPoseRequest_<ContainerAllocator> const>
  : FalseType
  { };

template <class ContainerAllocator>
struct IsMessage< ::vicon_bridge::viconGrabPoseRequest_<ContainerAllocator> >
  : TrueType
  { };

template <class ContainerAllocator>
struct IsMessage< ::vicon_bridge::viconGrabPoseRequest_<ContainerAllocator> const>
  : TrueType
  { };

template <class ContainerAllocator>
struct HasHeader< ::vicon_bridge::viconGrabPoseRequest_<ContainerAllocator> >
  : FalseType
  { };

template <class ContainerAllocator>
struct HasHeader< ::vicon_bridge::viconGrabPoseRequest_<ContainerAllocator> const>
  : FalseType
  { };


template<class ContainerAllocator>
struct MD5Sum< ::vicon_bridge::viconGrabPoseRequest_<ContainerAllocator> >
{
  static const char* value()
  {
    return "4045133337c2e7a711effc5b44dfbbb6";
  }

  static const char* value(const ::vicon_bridge::viconGrabPoseRequest_<ContainerAllocator>&) { return value(); }
  static const uint64_t static_value1 = 0x4045133337c2e7a7ULL;
  static const uint64_t static_value2 = 0x11effc5b44dfbbb6ULL;
};

template<class ContainerAllocator>
struct DataType< ::vicon_bridge::viconGrabPoseRequest_<ContainerAllocator> >
{
  static const char* value()
  {
    return "vicon_bridge/viconGrabPoseRequest";
  }

  static const char* value(const ::vicon_bridge::viconGrabPoseRequest_<ContainerAllocator>&) { return value(); }
};

template<class ContainerAllocator>
struct Definition< ::vicon_bridge::viconGrabPoseRequest_<ContainerAllocator> >
{
  static const char* value()
  {
    return "string subject_name\n\
string segment_name\n\
int32 n_measurements\n\
";
  }

  static const char* value(const ::vicon_bridge::viconGrabPoseRequest_<ContainerAllocator>&) { return value(); }
};

} // namespace message_traits
} // namespace ros

namespace ros
{
namespace serialization
{

  template<class ContainerAllocator> struct Serializer< ::vicon_bridge::viconGrabPoseRequest_<ContainerAllocator> >
  {
    template<typename Stream, typename T> inline static void allInOne(Stream& stream, T m)
    {
      stream.next(m.subject_name);
      stream.next(m.segment_name);
      stream.next(m.n_measurements);
    }

    ROS_DECLARE_ALLINONE_SERIALIZER
  }; // struct viconGrabPoseRequest_

} // namespace serialization
} // namespace ros

namespace ros
{
namespace message_operations
{

template<class ContainerAllocator>
struct Printer< ::vicon_bridge::viconGrabPoseRequest_<ContainerAllocator> >
{
  template<typename Stream> static void stream(Stream& s, const std::string& indent, const ::vicon_bridge::viconGrabPoseRequest_<ContainerAllocator>& v)
  {
    s << indent << "subject_name: ";
    Printer<std::basic_string<char, std::char_traits<char>, typename ContainerAllocator::template rebind<char>::other > >::stream(s, indent + "  ", v.subject_name);
    s << indent << "segment_name: ";
    Printer<std::basic_string<char, std::char_traits<char>, typename ContainerAllocator::template rebind<char>::other > >::stream(s, indent + "  ", v.segment_name);
    s << indent << "n_measurements: ";
    Printer<int32_t>::stream(s, indent + "  ", v.n_measurements);
  }
};

} // namespace message_operations
} // namespace ros

#endif // VICON_BRIDGE_MESSAGE_VICONGRABPOSEREQUEST_H
