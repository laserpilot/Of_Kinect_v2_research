/*
 * This file is part of the OpenKinect Project. http://www.openkinect.org
 *
 * Copyright (c) 2014 individual OpenKinect contributors. See the CONTRIB file
 * for details.
 *
 * This code is licensed to you under the terms of the Apache License, version
 * 2.0, or, at your option, the terms of the GNU General Public License,
 * version 2.0. See the APACHE20 and GPL2 files for the text of the licenses,
 * or the following URLs:
 * http://www.apache.org/licenses/LICENSE-2.0
 * http://www.gnu.org/licenses/gpl-2.0.txt
 *
 * If you redistribute this file in source form, modified or unmodified, you
 * may:
 *   1) Leave this header intact and distribute it under the same terms,
 *      accompanying it with the APACHE20 and GPL20 files, or
 *   2) Delete the Apache 2.0 clause and accompany it with the GPL2 file, or
 *   3) Delete the GPL v2 clause and accompany it with the APACHE20 file
 * In all cases you must keep the copyright notice intact and include a copy
 * of the CONTRIB file.
 *
 * Binary distributions must follow the binary distribution requirements of
 * either License.
 */

#ifndef RGB_PACKET_STREAM_PARSER_H_
#define RGB_PACKET_STREAM_PARSER_H_

#include <stddef.h>

#include "double_buffer.h"
#include "rgb_packet_processor.h"
#include "async_packet_processor.h"

#include "transfer_pool.h"

namespace libfreenect2
{

class RgbPacketStreamParser : public libfreenect2::usb::TransferPool::DataReceivedCallback
{
public:
  RgbPacketStreamParser(libfreenect2::RgbPacketProcessor* processor);
  virtual ~RgbPacketStreamParser();

  virtual void onDataReceived(unsigned char* buffer, size_t length);
private:
  libfreenect2::DoubleBuffer buffer_;
  libfreenect2::AsyncPacketProcessor<libfreenect2::RgbPacket, libfreenect2::RgbPacketProcessor> processor_;
};

} /* namespace libfreenect2 */
#endif /* RGB_PACKET_STREAM_PARSER_H_ */
