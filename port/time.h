#pragma once

// 该目录用于收敛平台差异，便于把 SUNNY_OS 移植到不同 RTOS/MCU。
// 建议按“最小可用接口”迭代：时间/日志/存储/互斥锁等按需添加。

namespace vsun::port {

// 输出格式固定为 "HH:MM"，字符串以 '\0' 结尾，out 长度必须 >= 6。
// 返回 false 表示当前平台无法提供时间（例如未配置 RTC/未同步）。
bool get_local_time_hhmm(char out[6]);

} // namespace vsun::port

