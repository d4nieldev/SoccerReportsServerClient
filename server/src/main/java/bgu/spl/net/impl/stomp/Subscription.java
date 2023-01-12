package bgu.spl.net.impl.stomp;

public class Subscription {
    private int subscriptionId;
    private int connectionId;
    private String topicName;

    public Subscription(int subscriptionId, int connectionId, String topicName){
        this.subscriptionId = subscriptionId;
        this.connectionId = connectionId;
        this.topicName = topicName;
    }

    public int getSubscriptionId() {
        return this.subscriptionId;
    }

    public int getConnectionId() {
        return this.connectionId;
    }

    public String getTopicName() {
        return this.topicName;
    }

    @Override
    public String toString() {
        return connectionId + " " + topicName + " " + subscriptionId;
    }
}
