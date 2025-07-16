#ifndef IMAGESPROCCESS_H_INCLUDED
#define IMAGESPROCCESS_H_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif
#include<stdint.h>
#include"DataConstants.h"
#include"ImagesDirections.h"



inline const uint8_t* ImageFromDirection(uint8_t direction)
{
  switch (direction)
  {
    case DirectionNone: return nullptr;
    case DirectionStart: return IMG_directionWaypoint;
    case DirectionEasyLeft: return IMG_directionEasyLeft;
    case DirectionEasyRight: return IMG_directionEasyRight;
    case DirectionEnd: return IMG_directionWaypoint;
    case DirectionVia: return IMG_directionWaypoint;
    case DirectionKeepLeft: return IMG_directionKeepLeft;
    case DirectionKeepRight: return IMG_directionKeepRight;
    case DirectionLeft: return IMG_directionLeft;
    case DirectionOutOfRoute: return IMG_directionOutOfRoute;
    case DirectionRight: return IMG_directionRight;
    case DirectionSharpLeft: return IMG_directionSharpLeft;
    case DirectionSharpRight: return IMG_directionSharpRight;
    case DirectionStraight: return IMG_directionStraight;
    case DirectionUTurnLeft: return IMG_directionUTurnLeft;
    case DirectionUTurnRight: return IMG_directionUTurnRight;
    case DirectionFerry: return IMG_directionFerry;
    case DirectionStateBoundary: return IMG_directionStateBoundary;
    case DirectionFollow: return IMG_directionFollow;
    case DirectionMotorway: return IMG_directionMotorway;
    case DirectionTunnel: return IMG_directionTunnel;
    case DirectionExitLeft: return IMG_directionExitLeft;
    case DirectionExitRight: return IMG_directionExitRight;
    case DirectionRoundaboutRSE: return IMG_directionRoundaboutRSE;
    case DirectionRoundaboutRE: return IMG_directionRoundaboutRE;
    case DirectionRoundaboutRNE: return IMG_directionRoundaboutRNE;
    case DirectionRoundaboutRN: return IMG_directionRoundaboutRN;
    case DirectionRoundaboutRNW: return IMG_directionRoundaboutRNW;
    case DirectionRoundaboutRW: return IMG_directionRoundaboutRW;
    case DirectionRoundaboutRSW: return IMG_directionRoundaboutRSW;
    case DirectionRoundaboutRS: return IMG_directionRoundaboutRS;
    case DirectionRoundaboutLSE: return IMG_directionRoundaboutLSE;
    case DirectionRoundaboutLE: return IMG_directionRoundaboutLE;
    case DirectionRoundaboutLNE: return IMG_directionRoundaboutLNE;
    case DirectionRoundaboutLN: return IMG_directionRoundaboutLN;
    case DirectionRoundaboutLNW: return IMG_directionRoundaboutLNW;
    case DirectionRoundaboutLW: return IMG_directionRoundaboutLW;
    case DirectionRoundaboutLSW: return IMG_directionRoundaboutLSW;
    case DirectionRoundaboutLS: return IMG_directionRoundaboutLS;
    case DirectionIMGHead: return IMG_goHead; 
  }
  return IMG_directionError;
}

/// Hàm chuyển đổi màu 4-bit sang 16-bit RGB565
inline uint16_t Color4To16bit(uint16_t color4bit)
{
  color4bit &= 0x0F;
  uint16_t color16bit = 0;
  const uint16_t maxColor4bit = 0x0F;
  const uint16_t maxColor5bit = 0x1F;
  const uint16_t maxColor6bit = 0x3F;
  const uint16_t red   = color4bit * maxColor5bit / maxColor4bit;
  const uint16_t green = color4bit * maxColor6bit / maxColor4bit;
  const uint16_t blue  = color4bit * maxColor5bit / maxColor4bit;
  //color 16 bit: rrrrrggg gggbbbbb
  color16bit |= red << 11;
  color16bit |= green << 5;
  color16bit |= blue;
  return color16bit;
}


inline uint8_t convertImage(uint32_t direct_code) {

  switch (direct_code)
  {
  case 0xBFF65DD7:
    return DirectionLeft;
  case 0x4982A8B3:
  case 1276508319:
    return DirectionRight;
  case 0x2E3F2593:
  case 1070899845:
    return DirectionStraight;
  case 0x7FBE5582:
    return DirectionUTurnLeft;
  default:
    return 0;
  }
  

}


#ifdef __cplusplus
}
#endif

#endif
