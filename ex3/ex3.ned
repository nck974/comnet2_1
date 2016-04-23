simple Reciever
{
    parameters:
      @signal[arrival](type="long");
      //@statistic[interarrivalTimeAccumulated](title="interarrivalTimeAccumulated"; source="arrival"; record=vector,stats; interpolationmode=none);
      //@statistic[lastArrivalTime](title="lastArrivalTime"; source="arrival"; record=vector,stats; interpolationmode=none);
      @statistic[currentInterarrival](title="currentInterarrival time"; source="arrival"; record=vector,stats; interpolationmode=none);
    gates:
        input in;
}

simple Sender {
  parameters:
    //volatile double delayTime @unit(s);
  gates:
    output out;
}

network Ex3Network
{
    submodules:
        sender: Sender;
        reciever: Reciever;
    connections:
        sender.out --> {  delay = 100ms; } --> reciever.in;
}
