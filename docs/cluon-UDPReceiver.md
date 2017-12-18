# How to receive bytes via a UDP socket

To receive data using a UDP socket, simply include the header `#include <cluon/UDPReceiver.hpp>`.

Next, create an instance of class `cluon::UDPReceiver` as follows: `cluon::UDPReceiver receiver("127.0.0.1", 1234, delegate);`.
The first parameter is of type `std::string` expecting a numerical IPv4 address, the second parameter
specifies the UDP port from which data shall be received from, and the last parameter is of type
`std::function` that is called whenever new bytes are available to be processed.

The complete signature for the delegate function is
`std::function<void(std::string &&, std::string &&, std::chrono::system_clock::time_point &&) noexcept>`:
The first parameter contains the bytes that have been received, the second parameter
containes the human-readable representation of the sender (X.Y.Z.W:ABCD), and the last
parameter is the time stamp when the data has been received. An example using a C++ lambda
expression would look as follows:

```c++
cluon::UDPReceiver receiver("127.0.0.1", 1234,
    [](std::string &&data, std::string &&sender, std::chrono::system_clock::time_point &&ts) noexcept {
        const auto timestamp(std::chrono::system_clock::to_time_t(ts));
        std::cout << "Received " << data.size() << " bytes from " << sender << " at "
                  << std::put_time(std::localtime(&timestamp), "%Y-%m-%d %X") << ", containing '" << data
                  << "'." << std::endl;
    });
```

After creating an instance of class `cluon::UDPReceiver`, it is immediately activated and
concurrently waiting for data. To check whether the instance was created successfully and
running, the method `isRunning()` can be called.

The code needs to be compiled using at least C++11 and linked with `libcluon` and `pthread`;
please refer to the example with the GCC compiler below:

```
g++ -std=c++11 -o UDPReceiverTest UDPReceiverTest.cpp -lcluon -pthread
```

A complete example is available [here](https://github.com/chrberger/libcluon/blob/master/libcluon/examples/cluon-UDPReceiver.cpp).
