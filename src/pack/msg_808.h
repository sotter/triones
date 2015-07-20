/**
 * author: Triones
 * date  : 2014-09-04
 */

#ifndef __TRIONES_MSG_808_H__
#define __TRIONES_MSG_808_H__

namespace triones
{

const uint16_t TERMINAL_COMMON_MSG = 0x0001; //终端通用应答
const uint16_t HEART_NOOP_MSG = 0x0002; //终端心跳
const uint16_t LOGOUT_INFO_MSG = 0x0003; //终端注销
const uint16_t TERMINAL_REG_MSG = 0x0100; //终端注册
const uint16_t TERMINAL_AUTH_MSG = 0x0102; //终端鉴权
const uint16_t QUERY_TERMINAL_PARAME_MSG = 0x0104; //查询终端参数应答
const uint16_t LOCATION_DATA_MSG = 0x0200; //终端位置上报
const uint16_t QUERY_LOCATION_INFO_MSG = 0x0201; //位置信息查询
const uint16_t EVENT_REPORT_MSG = 0x0301; //事件报告
const uint16_t QUESTION_REPLAY_MSG = 0x0302; //提问应答
const uint16_t INFO_DEMAND_CANCLE = 0x0303; //信息点播/取消
const uint16_t TERM_LOCATION_RESP_MSG = 0x0500; //车辆控制应答
const uint16_t TACHOGRAPH_BODY_MSG = 0x0700; //行驶记录仪
const uint16_t EWAYBILLREPORT_MSG = 0x0701; //电子运单
const uint16_t DRIVER_COLLECTION_INFO_MSG = 0x0702; //驾驶员身份信息采集上报
const uint16_t MEDIA_EVENT_REPORT = 0x0800; //多媒体事件上传
const uint16_t MEDIA_UPLOAD_DATA_MSG = 0x0801; //多媒体数据上传
const uint16_t DB_MEDIA_SEARCH_MSG = 0x0802; //存储多媒体检索应答
const uint16_t TRANS_DATA_MSG = 0x0900; //数据透传
const uint16_t DATA_COMPRESSION_MSG = 0x0901; //数据压缩
const uint16_t BLIND_SPORT_MSG = 0x0F10; //盲区补传
const uint16_t CAR_ANALYSIS_DATA_MSG = 0x0F11; //车辆分析数据
const uint16_t HISTORY_UPLOAD_DATA_MSG = 0x0F13; //历史数据上传
const uint16_t DRIVERACTION_EVENT_MSG = 0x0F14; //驾驶行为事件
const uint16_t TERMINAL_VERSION_MSG = 0x0F15; //终端版本信息
const uint16_t IDLE_SPEED_MSG = 0x0F18; //怠速参数
const uint16_t BEIDOU_UP_EXT_DATA_MSG = 0x7F00; //北斗扩展数据上报
const uint16_t BEIDOU_QUERY_EXT_REPLAY_MSG = 0x7F02; //查询扩展终端参数应答

const uint16_t QUERY_TERM_PROPERTY_ACK = 0x0107; //查询终端属性应答
const uint16_t TERM_UPGRADE_RESULT_NOTIFY = 0x0108;  //终端升级结果通知
const uint16_t LOCATION_DATA_BATCHING_UPSEND = 0x0704;  //定位数据的批量上传
const uint16_t CAN_DATA_UPSEND = 0x0705;  //can总线数据上传
const uint16_t CAMERA_IMMEDIATE_SHOOT_ACK = 0x0805; //摄像头立即拍摄命令应答

} // namespace triones

#endif // #ifndef __TRIONES_MSG_808_H__
