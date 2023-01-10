package bgu.spl.net.impl.stomp;

import java.io.IOException;
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
        boolean error = false;

        try {
            String command = frame.getCommand();
            if (command.equals("CONNECT"))
                processConnect(headers);
            else if (command.equals("SEND"))
                processSend(headers, frame.getBody());
            else if (command.equals("SUBSCRIBE"))
                processSubscribe(headers);
            else if (command.equals("UNSUBSCRIBE"))
                processUnSubscribe(headers);
            else if (command.equals("DISCONNECT"))
                processDisconnect();
            else
                // return error frame
                throw new StompException("Unidentified command");
        }
        catch (StompException ex){
            error = true;
            try{
                sendError(headers.getOrDefault("receipt", null), ex.getMessage());
            } catch (IOException e){
                shouldTerminate = true;
                e.printStackTrace();
            }
        }
        catch (IOException ex){
            shouldTerminate = true;
            ex.printStackTrace();
        }

        try{
            // send receipt if there was no error
            if (!error && headers.containsKey("receipt"))
                sendReceipt(headers.get("receipt"));
        }
        catch (IOException ex){
            shouldTerminate = true;
            ex.printStackTrace();
        }
    }

    private void sendReceipt(String receiptId) throws IOException {
        HashMap<String, String> headers = new HashMap<>();
        headers.put("receipt-id", receiptId);

        StompFrame s = new StompFrame("RECEIPT", headers, "");

        connections.send(connectionId, s.toString());
    }

    private void sendError(String receiptId, String message) throws IOException{
        
        HashMap<String, String> headers = new HashMap<>();
        if (receiptId != null)
            headers.put("receipt-id", receiptId);
        headers.put("message", message);

        StompFrame s = new StompFrame("ERROR", headers, "");
        System.out.println("SENDING ERROR TO CLIENT");
        System.out.println(s);
        connections.send(connectionId, s.toString());

        // close the connection
        shouldTerminate = true;
    }

    private void processConnect(HashMap<String, String> headers) throws StompException, IOException {   
        String version = headers.getOrDefault("accept-version", null);
        if (!version.equals("1.2"))
            throw new StompException("The server supports only version 1.2");
        if (!headers.containsKey("host"))
            throw new StompException("CONNECT command should contain a host header");
        if (!headers.containsKey("login"))
            throw new StompException("CONNECT command should contain a login header");
        if (!headers.containsKey("passcode"))
            throw new StompException("CONNECT command should contain a passcode header");
    
        Data.getInstance().connect(connectionId, headers.get("login"), headers.get("passcode"));

        // create the response
        StompFrame response = new StompFrame();
        response.setCommand("CONNECTED");
        response.addHeader("version", "1.2");

        // send response to client
        connections.send(connectionId, response.toString());
    }

    private void processSend(HashMap<String, String> headers, String body) throws StompException, IOException {
        if (!headers.containsKey("destination"))
            throw new StompException("SEND command should contain a destination header");
        
        String topic = headers.get("destination");

        // check if the subscription exists
        Integer subscriptionId = Data.getInstance().getSubscriptionId(connectionId, topic);
        if (subscriptionId == null)
            throw new StompException("This user is not subscribed to the topic. Try subscribing before sending a message");

        // create the stomp response
        StompFrame response = new StompFrame();
        response.setCommand("MESSAGE");
        response.addHeader("subscription", subscriptionId.toString());
        response.addHeader("message-id", Data.getInstance().getNextMessageId().toString());
        response.addHeader("destination", topic);
        response.setBody(body);

        // send the message to all topic subscribers
        connections.send(topic, response.toString());
    }

    private void processSubscribe(HashMap<String, String> headers) throws StompException{
        if (!headers.containsKey("destination"))
            throw new StompException("SUBSCRIBE command should contain a destination header");
        if (!headers.containsKey("id"))
            throw new StompException("SUBSCRIBE command should contain an id header");

        Data.getInstance().subscribe(connectionId, headers.get("destination"), Integer.parseInt(headers.get("id")));
    }

    private void processUnSubscribe(HashMap<String, String> headers) throws StompException{
        if (!headers.containsKey("id"))
            throw new StompException("UNSUBSCRIBE command should contain an id header");

        Data.getInstance().unSubscribe(connectionId, Integer.parseInt(headers.get("id")));
    }

    private void processDisconnect() throws StompException{
        Data.getInstance().disconnect(connectionId);
        connections.disconnect(connectionId);
    }

    @Override
    public boolean shouldTerminate() {
        return shouldTerminate;
    }
}
