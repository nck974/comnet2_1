#include <string.h>
#include <omnetpp.h>
#include <math.h>


class Lin_Con_Rand { //Linear Congruential Random Number Generator
public:
  double mult = 16807;
  double m =pow(2,31)-1;
  double generateNumber( double seed){
    double randNum = fmod(( seed * mult ), m); //fmod = "%" for double
    return randNum;
  }
};

class uniformDistribution { //uniform distributed Inter-arrival time T (e.g. uniform (0,2) using Lin_Con_Rand
public:
  double uniformValue (double min, double max, double currentNumber) {
    double maxNumber=pow(2,31)-2;
    return ((max*currentNumber)/(maxNumber-min));
  }
};

class exponentialDistribution { //Compute inverse exponential distribution
public:
  double expValue (double lamda, double uniformValue){
    return -1/lamda*log(1-uniformValue);
  }
};



class Sender : public cSimpleModule {
protected:
  virtual void initialize() override {
    // Get delay time as random value from exponential distribution as set in omnetpp.ini.
    //simtime_t delay = par("delayTime");
    currentNumber=1;
    currentNumber=randGenerator.generateNumber(currentNumber);
    currentUniformValue= Dist.uniformValue(0,1,currentNumber); //Map random number into uniform function

    simtime_t delay = expDist.expValue(1,currentUniformValue); //Get x where exponential cumulative function is the random value

    scheduleAt(simTime() + delay, new cMessage("selfmsg"));
  }
  virtual void handleMessage(cMessage *msg) override {
    delete msg;
    //simtime_t delay = par("delayTime");
    currentNumber=randGenerator.generateNumber(currentNumber);
    currentUniformValue= Dist.uniformValue(0,1,currentNumber); //Map random number into cumulative function

    simtime_t delay = expDist.expValue(1,currentUniformValue);  //Get x where exponential cumulative function is the random value

    EV << "Delay expired, sending another message. New delay is: " << delay << endl;
    // Send out a message to the reciever.
    send(new cMessage("msg"), "out");
    // Schedule a self message after delay.
    scheduleAt(simTime() + delay, new cMessage("selfmsg"));
  }
private:
  double currentNumber;
  double currentUniformValue;
  exponentialDistribution expDist;
  Lin_Con_Rand randGenerator;
  uniformDistribution Dist;
};



Define_Module(Sender);

class Reciever : public cSimpleModule {
protected:
  virtual void initialize() override {
    arrivalSignal = registerSignal("arrival"); //Initialize signal
    lastArrivalTime = 0.0;
    interarrivalTimeAccumulated = 0.0;
    messageCount = 0;
  }
  virtual void handleMessage(cMessage *msg) override {
    delete msg;
    messageCount++;
    interarrivalTimeAccumulated += simTime().dbl() - lastArrivalTime;
    lastArrivalTime = simTime().dbl();
    //emit(arrivalSignal, interarrivalTimeAccumulated); //Trigger signal to save data
    //emit(arrivalSignal, lastArrivalTime); //Trigger signal to save data
    double currentInterarrival = interarrivalTimeAccumulated/messageCount;
    emit(arrivalSignal, currentInterarrival); //Trigger signal to save data
    EV << "Recieved message. Current interarrival time is: " << interarrivalTimeAccumulated / messageCount << "s" <<  endl;
  }
private:
  double lastArrivalTime;
  double interarrivalTimeAccumulated;
  int messageCount;
  simsignal_t arrivalSignal; //Signal: message has arrived
};

Define_Module(Reciever);
