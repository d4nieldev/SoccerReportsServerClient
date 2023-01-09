package bgu.spl.net.impl.stomp;

import java.util.HashMap;

import org.w3c.dom.html.HTMLFrameSetElement;

import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.srv.ConnectionHandler;
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
        String errorMsg = null;

        switch (frame.getCommand()){
            case "CONNECT":
                errorMsg = processConnect(headers);
            case "SEND":
                break;
            case "SUBSCRIBE":
                break;
            case "UNSUBSCRIBE":
                break;
            case "DISCONNECT":
                break;
            default:
                // return error frame
                break;
        }

        if (errorMsg == null && headers.containsKey("receipt"))
            sendReceipt(headers.get("receipt"));
        else if (errorMsg != null)
            sendError(headers.get("receipt"), errorMsg);
        
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
    }

    private String processConnect(HashMap<String, String> headers) {
        String version = headers.getOrDefault("accept-version", null);
        if (version != "1.2")
            throw new IllegalArgumentException("The server supports only version 1.2");
    
        switch (connections.connect(connectionId, headers.get("host"), headers.get("login"), headers.get("passcode"))){
            case "USER_ERR":
                return "User already logged in";
            case "CLIENT_ERR":
                // TODO maybe pass to the client
                return "The client is already logged in, logout before trying again";
            case "PASS_ERR":
                return "Wrong Password";
            case "SUCCESS":
                HashMap<String, String> respHeaders = new HashMap<>();
                respHeaders.put("version", "1.2");
                StompFrame response = new StompFrame("CONNECTED", respHeaders, "");
                connections.send(connectionId, response.toString());
                return null;
            default:
                return "The client is not active";
        }
    }

    @Override
    public boolean shouldTerminate() {
        return shouldTerminate;
    }
}
