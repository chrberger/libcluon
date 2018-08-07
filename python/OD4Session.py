# Copyright (C) 2017-2018 Christian Berger, Julian Scholle
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

import datetime
import time
import signal
import socket
import struct
import thread

import cluonDataStructures_pb2

# This class provides an OD4Session listener to receive live messages from a running OD4Session.
class OD4Session:
    @staticmethod
    def run():
        signal.pause()


    def __init__(self, cid, port=12175):
        assert cid <= 255
        self.MULTICAST_PORT = port
        self.MULTICAST_GROUP = "225.0.0." + str(cid)
        self.isRunning = False
        self.isConnected = False
        self.callbacks = dict()
        self.sock = None


    def connect(self):
        assert not self.isConnected
        # Create UDP multicast socket.
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.sock.bind(('', self.MULTICAST_PORT))
        req = struct.pack("4sl", socket.inet_aton(self.MULTICAST_GROUP), socket.INADDR_ANY)
        self.sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, req)
        self.isConnected = True
        if not self.isRunning:
            thread.start_new_thread(self.__runner, ())
            self.isRunning = True


    def send(self, messageID, rawStringFromMessageToSend):
        now = time.time()

        sentTimeStamp = cluonDataStructures_pb2.cluon_data_TimeStamp()
        sentTimeStamp.seconds = int(now)
        sentTimeStamp.microseconds = int((now - int(now))*1000*1000)

        envelope = cluonDataStructures_pb2.cluon_data_Envelope()
        envelope.dataType = messageID
        envelope.sent.seconds = sentTimeStamp.seconds
        envelope.sent.microseconds = sentTimeStamp.microseconds
        envelope.sampleTimeStamp.seconds = sentTimeStamp.seconds
        envelope.sampleTimeStamp.microseconds = sentTimeStamp.microseconds
        envelope.serializedData = rawStringFromMessageToSend

        serializedEnvelope = envelope.SerializeToString()
        size = len(serializedEnvelope);

        # Add Envelope header.
        a = struct.pack("<B", *bytearray([0x0D, ]))
        b = struct.pack("<L", ((size & 0xFFFFFF) << 8) | 0xA4)

        data = a + b + serializedEnvelope
        self.sock.sendto(data, (self.MULTICAST_GROUP, self.MULTICAST_PORT))


    def registerMessageCallback(self, msgID, func, msgType, params=()):
        assert hasattr(func, '__call__')
        self.callbacks[msgID] = (func, msgType, params)


    def __process(self, e):
        print "Received Envelope with ID = " + str(e.dataType)

        # Extract sent, received, and sample time point.
        sent = datetime.datetime.fromtimestamp(timestamp=e.sent.seconds) + datetime.timedelta(microseconds=e.sent.microseconds)
        received = datetime.datetime.fromtimestamp(timestamp=e.received.seconds) + datetime.timedelta(microseconds=e.received.microseconds)
        sampleTimeStamp = datetime.datetime.fromtimestamp(timestamp=e.sampleTimeStamp.seconds) + datetime.timedelta(microseconds=e.sampleTimeStamp.microseconds)
        timestamps = [sent, received, sampleTimeStamp]

        # Check for registered callbacks.
        if e.dataType in self.callbacks.keys():
            msg = self.callbacks[e.dataType][1]()
            msg.ParseFromString(e.serializedData)
            thread.start_new_thread(self.callbacks[e.dataType][0], (msg, timestamps) + (self.callbacks[e.dataType][2]))


    def __runner(self):
        LENGTH_ENVELOPE_HEADER = 5
        buf = ""
        expectedBytes = 0
        consumedEnvelopeHeader = False

        while True:
            if consumedEnvelopeHeader:
                if len(buf) >= expectedBytes:
                    envelope = cluonDataStructures_pb2.cluon_data_Envelope()
                    envelope.ParseFromString(buf)
                    self.__process(envelope)
                    # Start over and read next container.
                    consumedEnvelopeHeader = False
                    buf = buf[expectedBytes:]
                    expectedBytes = 0

            if not consumedEnvelopeHeader:
                if len(buf) >= LENGTH_ENVELOPE_HEADER:
                    consumedEnvelopeHeader = True
                    byte0 = buf[0]
                    byte1 = buf[1]

                    # Check for Envelope header.
                    if ord(byte0) == int('0x0D',16) and ord(byte1) == int('0xA4',16):
                        v = struct.unpack('<L', buf[1:5]) # Read uint32_t and convert to little endian.
                        expectedBytes = v[0] >> 8 # The second byte belongs to the header of an Envelope.
                        buf = buf[5:] # Remove header.
                    else:
                        print "Failed to consume header from Envelope."

            # Receive data from UDP socket.
            data = self.sock.recv(2048)
            if len(data) != 2048:
                # Avoid corrupt packets.
                buf = buf + data

