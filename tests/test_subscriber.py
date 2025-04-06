import sys
import signal

sys.path.append("/usr/local/lib/python3/dist-packages/")
sys.path.append("build/dcps/sturdds/")
import fastdds
import NavMessage


def signal_handler(sig, frame):
    print("Interrupted!")


class Listener(fastdds.DataReaderListener):
    def __init__(self):
        super().__init__()

    def on_data_available(self, reader):
        info = fastdds.SampleInfo()
        data = NavMessage.NavMessage()
        if reader.take_next_sample(data, info) == fastdds.RETCODE_OK:
            print("Received Message: ")
            print("----------------- ")
            print(f"Index = {data.stamp().index()}")
            print(f"Valid = {data.stamp().valid()}")
            print(f"Sec   = {data.stamp().sec()}")
            print(f"NSec  = {data.stamp().nanosec()}")
            print(f"Lat   = {data.Lat()}")
            print(f"Lon   = {data.Lon()}")
            print(f"H     = {data.H()}")
            print(f"Vn    = {data.Vn()}")
            print(f"Ve    = {data.Ve()}")
            print(f"Vd    = {data.Vd()}")
            print(f"Roll  = {data.Roll()}")
            print(f"Pitch = {data.Pitch()}")
            print(f"Yaw   = {data.Yaw()}")
            print(f"Bias  = {data.Bias()}")
            print(f"Drift = {data.Drift()}")
            print(f"P0    = {data.P0()}")
            print(f"P1    = {data.P1()}")
            print(f"P2    = {data.P2()}")
            print(f"P3    = {data.P3()}")
            print(f"P4    = {data.P4()}")
            print(f"P5    = {data.P5()}")
            print(f"P6    = {data.P6()}")
            print(f"P7    = {data.P7()}")
            print(f"P8    = {data.P8()}")
            print(f"P9    = {data.P9()}")
            print(f"P10   = {data.P10()}")

    def on_subscription_matched(self, reader, info):
        if info.current_count_change == 1:
            print(f"Subscriber matched a publisher (Reader: {reader.guid()})")
        elif info.current_count_change == -1:
            print(f"Subscriber unmatched a publisher (Reader: {reader.guid()})")


class Subscriber:
    def __init__(self, topic_name):
        factory = fastdds.DomainParticipantFactory.get_instance()
        self.participant_qos = fastdds.DomainParticipantQos()
        factory.get_default_participant_qos(self.participant_qos)
        self.participant = factory.create_participant(0, self.participant_qos)

        self.topic_data_type = NavMessage.NavMessagePubSubType()
        self.topic_data_type.set_name("NavMessage")
        self.type_support = fastdds.TypeSupport(self.topic_data_type)
        self.participant.register_type(self.type_support)

        self.topic_qos = fastdds.TopicQos()
        self.participant.get_default_topic_qos(self.topic_qos)
        self.topic = self.participant.create_topic(
            topic_name, self.topic_data_type.get_name(), self.topic_qos
        )

        self.subscriber_qos = fastdds.SubscriberQos()
        self.participant.get_default_subscriber_qos(self.subscriber_qos)
        self.subscriber = self.participant.create_subscriber(self.subscriber_qos)

        self.listener = Listener()
        self.reader_qos = fastdds.DataReaderQos()
        self.subscriber.get_default_datareader_qos(self.reader_qos)
        self.reader = self.subscriber.create_datareader(self.topic, self.reader_qos, self.listener)

    def __del__(self):
        factory = fastdds.DomainParticipantFactory.get_instance()
        self.participant.delete_contained_entities()
        factory.delete_participant(self.participant)

    def run(self):
        signal.signal(signal.SIGINT, signal_handler)
        print("Press Ctrl+C to stop")
        signal.pause()


if __name__ == "__main__":
    node = Subscriber("navigator")
    node.run()
