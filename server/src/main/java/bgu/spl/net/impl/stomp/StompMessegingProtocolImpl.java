package bgu.spl.net.impl.stomp;

import java.util.HashMap;

import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.srv.Connections;

public class StompMessegingProtocolImpl implements StompMessagingProtocol<String> {

    private int connectionId;
    private Connections<String> connections;
    private boolean shouldTerminate;

    @Override
    public void start(int connectionId, Connections<String> connections) {
        this.connectionId = connectionId;
        this.connections = connections;
    }

    @Override
    public void process(String message) {
        StompFrame frame = new StompFrame(message);
        HashMap<String, String> headers = frame.getHeaders();
        String errorMsg;

        switch (frame.getCommand()){
            case "CONNECT":
                errorMsg = processConnect(headers);
            case "SEND":
                errorMsg = processSend(headers, frame.getBody());
            case "SUBSCRIBE":
                errorMsg = processSubscribe(headers);
            case "UNSUBSCRIBE":
                errorMsg = processUnSubscribe(headers);
            case "DISCONNECT":
                errorMsg = processDisconnect();
            default:
                // return error frame
                errorMsg = "Unidentified command.";
        }

        // send receipt if there was no error
        if (errorMsg == null && headers.containsKey("receipt"))
            sendReceipt(headers.get("receipt"));
        // send error if an error occured
        else if (errorMsg != null)
            sendError(headers.getOrDefault("receipt", null), errorMsg);
        
    }

    private void sendReceipt(String receiptId){
        HashMap<String, String> headers = new HashMap<>();
        headers.put("receipt-id", receiptId);

        StompFrame s = new StompFrame("RECEIPT", headers, "");

        connections.send(connectionId, s.toString());
    }

    private void sendError(String receiptId, String message){
        HashMap<String, String> headers = new HashMap<>();
        if (receiptId != null)
            headers.put("receipt-id", receiptId);
        headers.put("message", message);

        StompFrame s = new StompFrame("ERROR", headers, "");

        connections.send(connectionId, s.toString());

        // close the connection
        shouldTerminate = true;
    }

    private String processConnect(HashMap<String, String> headers) {
        String version = headers.getOrDefault("accept-version", null);
        if (version != "1.2")
            return "The server supports only version 1.2";
        if (!headers.containsKey("host"))
            return "CONNECT command should contain a host header";
        if (!headers.containsKey("login"))
            return "CONNECT command should contain a login header";
        if (!headers.containsKey("passcode"))
            return "CONNECT command should contain a passcode header";
    
        switch (Data.getInstance().connect(connectionId, headers.get("login"), headers.get("passcode"))){
            case "USER_ERR":
                return "User already logged in";
            case "CLIENT_ERR":
                // TODO maybe pass to the client
                return "The client is already logged in, logout before trying again";
            case "PASS_ERR":
                return "Wrong Password";
            case "SUCCESS":
                // create the response
                StompFrame response = new StompFrame();
                response.setCommand("CONNECTED");
                response.addHeader("version", "1.2");

                // send response to client
                connections.send(connectionId, response.toString());

                return null;
            default:
                return "Unidentified error at CONNECT";
        }
    }

    private String processSend(HashMap<String, String> headers, String body){
        if (!headers.containsKey("destination"))
            return "SEND command should contain a destination header";
        
        String topic = headers.get("destination");

        // check if the subscription exists
        Integer subscriptionId = Data.getInstance().getSubscriptionId(connectionId, topic);
        if (subscriptionId == null)
            return "This user is not subscribed to the topic. Try subscribing before sending a message";

        // create the stomp response
        StompFrame response = new StompFrame();
        response.setCommand("MESSAGE");
        response.addHeader("subscription", subscriptionId.toString());
        response.addHeader("message-id", Data.getInstance().getNextMessageId().toString());
        response.addHeader("destination", topic);
        response.setBody(body);

        // send the message to all topic subscribers
        connections.send(topic, response.toString());

        return null;
    }

    private String processSubscribe(HashMap<String, String> headers){
        if (!headers.containsKey("destination"))
            return "SUBSCRIBE command should contain a destination header";
        if (!headers.containsKey("id"))
            return "SUBSCRIBE command should contain an id header";

        switch(Data.getInstance().subscribe(connectionId, headers.get("destination"), Integer.parseInt(headers.get("id")))){
            case "CLIENT_ERR":
                return "The client is not logged in";
            case "EXISTING_SUB_TOPIC_ERR":
                return "the user is already subscribed to the destination topic";
            case "EXISTING_SUB_ID_ERR":
                return "the user already has a subscription with this id";
            case "SUCCESS":
                // the output for this is only the receipt
                return null;
            default:
                return "Unidentified error at SUBSCRIBE";
        }
    }

    private String processUnSubscribe(HashMap<String, String> headers){
        if (!headers.containsKey("id"))
            return "UNSUBSCRIBE command should contain an id header";

        switch(Data.getInstance().unSubscribe(connectionId, Integer.parseInt(headers.get("id")))){
            case "CLIENT_ERR":
                return "the client is not logged in";
            case "SUBID_ERR":
                return "there is no subscription with the subscription id specified";
            case "SUCCESS":
                // the output for this is only the receipt
                return null;
            default:
                return "Unidentified error at UNSUBSCRIBE";
        }
    }

    private String processDisconnect(){
        switch(Data.getInstance().disconnect(connectionId)){
            case "CLIENT_ERR":
                return "The client is not logged in";
            case "SUCCESS":
                connections.disconnect(connectionId);
                shouldTerminate = true;
                return null;
            default:
                return "Unidentified error at DISCONNECT";
        }
    }

    @Override
    public boolean shouldTerminate() {
        return shouldTerminate;
    }
}
