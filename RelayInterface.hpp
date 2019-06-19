/*
   Interface to relay hardware.

   This is a RelayInterface base class that is not intended
   for use but provides the interface functions.  To create
   a usable relay interface, inherit from RelayInterface and
   define kRelayPin.  The functions can then be used via
   calls to static methods.
*/

class RelayInterface {
  public:
    RelayInterface() = delete;
    virtual ~RelayInterface();
    static void initialize();
    static void turnOn();
    static void turnOff();
    // static bool isOn();
  protected:
    static const int kRelayPin = -1;
};
