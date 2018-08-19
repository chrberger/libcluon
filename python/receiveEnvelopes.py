#!/usr/bin/env python2

# Copyright (C) 2018  Christian Berger
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

# This is the Python version of libcluon's OD4Session:
import OD4Session

# This is our example message specification.
import MyExampleMessageSpec_pb2

# Callback for a message of interest.
def onMessage(msg, timeStamps):
   print "sent: " + str(timeStamps[0]) + ", received: " + str(timeStamps[1]) + ", sample time stamps: " + str(timeStamps[2])
   print msg

# "Main" part.
session = OD4Session.OD4Session(cid=253) # Connect to running OD4Session at CID 253.
session.registerMessageCallback(30005, onMessage, MyExampleMessageSpec_pb2.my_TestMessage)
session.connect()
session.run()
