package bgu.spl.net.impl.stomp;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.atomic.AtomicInteger;

public class Data {
    private static Data instance = new Data();
    // maps between a LOGGED IN USER to the relevant connection id
    private ConcurrentHashMap<String, Integer> loggedInUsers;

    // maps between a LOGGED IN CLIENT (connectionId) to the relevant username
    private ConcurrentHashMap<Integer, String> loggedInClients;

    // maps between a username to its passcode
    private ConcurrentHashMap<String, String> users;

    // maps between a topic name to its subscribers (connection ids)
    private ConcurrentHashMap<String, List<Integer>> topicsToConnectionIds;

    // maps between a connection id (user) to his subscribed topics
    private ConcurrentHashMap<Integer, List<Subscription>> connectionIdsToSubscriptions;

    // represents the id of the next message
    private AtomicInteger nextMessageId;

    private Data() {
        this.loggedInUsers = new ConcurrentHashMap<>();
        this.loggedInClients = new ConcurrentHashMap<>();

        this.users = new ConcurrentHashMap<>();

        this.topicsToConnectionIds = new ConcurrentHashMap<>();
        this.connectionIdsToSubscriptions = new ConcurrentHashMap<>();

        this.nextMessageId = new AtomicInteger(0);
    }

    public static Data getInstance(){
        return instance;
    }

    /**
     * Checks if a user is registered to a topic
     * @param connectionId The connection id to identify the user (or client)
     * @param topic        The topic to check registration with
     * @return true iff the user is registered to the topic
     */
    private boolean isConnectionIdRegisteredToTopic(int connectionId, String topic){
        for(Subscription s : connectionIdsToSubscriptions.get(connectionId))
            if (s.getTopicName() == topic)
                return true;
        
        return false;
    }

    /**
     * Checks if a subscription id is a subscription of the user
     * @param subscriptionId The subscription id
     * @param connectionId   The connection id to identify the user (or client)
     * @return true iff the subscription id is a subscription of the user
     */
    private boolean isSubscriptionIdOfUser(int subscriptionId, int connectionId){
        for(Subscription s : connectionIdsToSubscriptions.get(connectionId))
            if (s.getSubscriptionId() == subscriptionId)
                return true;
        
        return false;
    }

    /**
     * logs the user into the server
     * @param connectionId The connection id
     * @param login        The username
     * @param passcode     The password
     * @return "USER_ERR"   iff the user is already logged in from some client
     *          <br/><br/>
     *         "CLIENT_ERR" iff the client is already logged in
     *          <br/><br/>
     *         "PASS_ERR"   iff wrong password
     *          <br/><br/>
     *         "SUCCESS"    iff the login was successful
     */
    public synchronized String connect(int connectionId, String login, String passcode){
        // the user is logged in from some client
        if (loggedInUsers.contains(login))
            return "USER_ERR";
        // TODO maybe handle on client side
        // the client has some user logged in from it
        else if (loggedInClients.contains(connectionId))
            return "CLIENT_ERR";
        // wrong passcode
        else if (users.computeIfAbsent(login, l -> passcode) != passcode)
            return "PASS_ERR";

        loggedInUsers.put(login, connectionId);
        loggedInClients.put(connectionId, login);

        connectionIdsToSubscriptions.put(connectionId, new ArrayList<>());

        return "SUCCESS";
    }

    /**
     * Registeres the client (or user) to the destination provided
     * @param connectionId   The connection id to identify the client (or user)
     * @param destination    The topic that the client (or user) is requesting to subscribe to
     * @param subscriptionId The subscription id provided by the client
     * @return "CLIENT_ERR" iff the client is not logged in
     *          <br/><br/>
     *          "EXISTING_SUB_TOPIC_ERR" iff the user is already subscribed to the destination topic
     *          <br/><br/>
     *          "EXISTING_SUB_ID_ERR" iff the user already has a subscription with this id
     *          <br/><br/>
     *          "SUCCESS" iff the subscription was successful
     */
    public String subscribe(int connectionId, String destination, int subscriptionId){
        if (!loggedInClients.contains(connectionId))
            return "CLIENT_ERR";
        else if (isConnectionIdRegisteredToTopic(connectionId, destination))
            return "EXISTING_SUB_TOPIC_ERR";
        else if (isSubscriptionIdOfUser(subscriptionId, connectionId))
            return "EXISTING_SUB_ID_ERR";
    
        // register the subscription at the user
        Subscription s = new Subscription(subscriptionId, connectionId, destination);
        connectionIdsToSubscriptions.get(connectionId).add(s);

        // register the user at the topic subscribers (create one if the topic is absent)
        topicsToConnectionIds.computeIfAbsent(destination, d -> new ArrayList<>()).add(connectionId);

        return "SUCCESS";
    }

    /**
     * Removes a subscription
     * @param connectionId   The id of the user (or client)
     * @param subscriptionId The id of the subscription to remove
     * @return  "CLIENT_ERR" iff the client is not logged in
     *          <br/><br/>
     *          "SUBID_ERR" iff there is no subscription of the user with the subscription id specified
     *          <br/><br/>
     *          "SUCCESS" iff the subscription was removed successfully
     */
    public String unSubscribe(int connectionId, int subscriptionId){
        if (!loggedInClients.contains(connectionId))
            return "CLIENT_ERR";
            
        // search and remove the subscription from the user if found
        Subscription subToRemove = null;
        for (Subscription s : connectionIdsToSubscriptions.get(connectionId))
            if (s.getConnectionId() == connectionId){
                subToRemove = s;
                break;
            }

        // did not find the subscription
        if (subToRemove == null)
            return "SUBID_ERR";

        // remove the user from the topic subscribers
        topicsToConnectionIds.get(subToRemove.getTopicName()).remove(connectionId);

        return "SUCCESS";
    }

    /**
     * Disconnects a user and his client from the server.
     * Removes the user subscription from each topic.
     * @param connectionId the connection id to identify the client (and user)
     * @return "CLIENT_ERR" iff the client is not logged in
     *          <br/><br/>
     *         "SUCCESS" iff disconnected the user
     */
    public String disconnect(int connectionId){
        if (!loggedInClients.contains(connectionId))
            return "CLIENT_ERR";
    
        // unsubscribe from each topic
        for (Subscription s: connectionIdsToSubscriptions.get(connectionId))
            unSubscribe(connectionId, s.getSubscriptionId());
        
        // remove user
        connectionIdsToSubscriptions.remove(connectionId);

        // logout the user and client
        loggedInUsers.remove(loggedInClients.remove(connectionId));

        return "SUCCESS";
    }

    /**
     * Returns the subscription id that corresponds to the connection id and the topic
     * @param connectionId The connection id to identify the client (or user)
     * @param topic        The topic name
     * @return the corresponding subscription id if the subscription exists, null otherwise
     */
    public Integer getSubscriptionId(int connectionId, String topic){
        for(Subscription s : connectionIdsToSubscriptions.get(connectionId)) {
            if (s.getTopicName() == topic)
                return s.getSubscriptionId();
        }
        return null;
    }

    public Integer getNextMessageId(){
        return nextMessageId.getAndIncrement();
    }

    /**
     * 
     * @param topic The topic to get all subscribers from
     * @return a list of all the connection ids who are subscribed to the topic
     */
    public List<Integer> getSubscribers(String topic){
        return topicsToConnectionIds.get(topic);
    }
}
