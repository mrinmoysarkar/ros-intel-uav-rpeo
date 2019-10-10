// Generated by gencpp from file ardrone_autonomy/LedAnim.msg
// DO NOT EDIT!


#ifndef ARDRONE_AUTONOMY_MESSAGE_LEDANIM_H
#define ARDRONE_AUTONOMY_MESSAGE_LEDANIM_H

#include <ros/service_traits.h>


#include <ardrone_autonomy/LedAnimRequest.h>
#include <ardrone_autonomy/LedAnimResponse.h>


namespace ardrone_autonomy
{

struct LedAnim
{

typedef LedAnimRequest Request;
typedef LedAnimResponse Response;
Request request;
Response response;

typedef Request RequestType;
typedef Response ResponseType;

}; // struct LedAnim
} // namespace ardrone_autonomy


namespace ros
{
namespace service_traits
{


template<>
struct MD5Sum< ::ardrone_autonomy::LedAnim > {
  static const char* value()
  {
    return "0447d1620f8ba70a5b1fc2d89e406549";
  }

  static const char* value(const ::ardrone_autonomy::LedAnim&) { return value(); }
};

template<>
struct DataType< ::ardrone_autonomy::LedAnim > {
  static const char* value()
  {
    return "ardrone_autonomy/LedAnim";
  }

  static const char* value(const ::ardrone_autonomy::LedAnim&) { return value(); }
};


// service_traits::MD5Sum< ::ardrone_autonomy::LedAnimRequest> should match 
// service_traits::MD5Sum< ::ardrone_autonomy::LedAnim > 
template<>
struct MD5Sum< ::ardrone_autonomy::LedAnimRequest>
{
  static const char* value()
  {
    return MD5Sum< ::ardrone_autonomy::LedAnim >::value();
  }
  static const char* value(const ::ardrone_autonomy::LedAnimRequest&)
  {
    return value();
  }
};

// service_traits::DataType< ::ardrone_autonomy::LedAnimRequest> should match 
// service_traits::DataType< ::ardrone_autonomy::LedAnim > 
template<>
struct DataType< ::ardrone_autonomy::LedAnimRequest>
{
  static const char* value()
  {
    return DataType< ::ardrone_autonomy::LedAnim >::value();
  }
  static const char* value(const ::ardrone_autonomy::LedAnimRequest&)
  {
    return value();
  }
};

// service_traits::MD5Sum< ::ardrone_autonomy::LedAnimResponse> should match 
// service_traits::MD5Sum< ::ardrone_autonomy::LedAnim > 
template<>
struct MD5Sum< ::ardrone_autonomy::LedAnimResponse>
{
  static const char* value()
  {
    return MD5Sum< ::ardrone_autonomy::LedAnim >::value();
  }
  static const char* value(const ::ardrone_autonomy::LedAnimResponse&)
  {
    return value();
  }
};

// service_traits::DataType< ::ardrone_autonomy::LedAnimResponse> should match 
// service_traits::DataType< ::ardrone_autonomy::LedAnim > 
template<>
struct DataType< ::ardrone_autonomy::LedAnimResponse>
{
  static const char* value()
  {
    return DataType< ::ardrone_autonomy::LedAnim >::value();
  }
  static const char* value(const ::ardrone_autonomy::LedAnimResponse&)
  {
    return value();
  }
};

} // namespace service_traits
} // namespace ros

#endif // ARDRONE_AUTONOMY_MESSAGE_LEDANIM_H
