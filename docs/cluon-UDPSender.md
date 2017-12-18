# How to send bytes via a UDP socket

To send data using a UDP socket, simply include the header `#include <cluon/UDPSender.hpp>`.

Next, create an instance of class `cluon::UDPSender` as follows: `cluon::UDPSender sender("127.0.0.1", 1234);`. The first parameter is of type `std::string` expecting a numerical IPv4 address and the second parameter specifies the UDP port to which the data shall be sent to.

To finally send data, simply call the method `send` supplying the data to be sent: `sender.send(std::move("Hello World!")`. Please note that the data is supplied using the _move_-semantics. The method `send` returns a `std::pair<ssize_t, int32_t>` where the first element returns the size of the successfully sent bytes and the second element contains the error code in case the transmission of the data failed.

```c++
cluon::UDPSender sender("127.0.0.1", 1234);

std::pair<ssize_t, int32_t> retVal = sender.send(std::move("Hello World!"));

std::cout << "Send " << retVal.first << " bytes, error code = " << retVal.second << std::endl;
```

The code needs to be compiled using at least C++11 and linked with `libcluon` and `pthread`; please refer to the example with the GCC compiler below:

```
g++ -std=c++11 -o UDPSenderTest UDPSenderTest.cpp -lcluon -pthread
```

A complete example is available [here](https://github.com/chrberger/libcluon/blob/master/libcluon/examples/cluon-UDPSender.cpp).
