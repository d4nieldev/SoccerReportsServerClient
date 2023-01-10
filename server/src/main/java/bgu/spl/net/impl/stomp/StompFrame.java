package bgu.spl.net.impl.stomp;

import java.util.HashMap;

public class StompFrame {
    private String command;
    private HashMap<String, String> headers;
    private String body;

    public StompFrame(String message)
    {
        if (message.endsWith("\u0000"))
            throw new IllegalArgumentException("Stomp frame must end with " + '\u0000');
        
        String pureMessage = "";
        for (int i = 0; i < message.length() && message.charAt(i) != '\u0000'; i++)
            pureMessage += message.charAt(i);

        headers = new HashMap<>();
        
        String[] lines = pureMessage.split("\n");
        command = lines[0];

        int pos = 1;
        for (pos = 1; pos < lines.length && !lines[pos].isBlank(); pos++){
            String[] header = lines[pos].split(":");
            headers.put(header[0], header[1]);
        }

        // avoid the blank line
        pos++;

        body = "";
        for (;pos < lines.length; pos++){
            body += lines[pos];
            if (pos != lines.length - 1)
                body += "\n";
        }
    }

    public StompFrame(String command, HashMap<String, String> headers, String body){
        this.command = command;
        this.headers = headers;
        this.body = body;
    }

    public StompFrame(){
        this.command = "";
        this.headers = new HashMap<>();
        this.body = "";
    }

    @Override
    public String toString(){
        String output = command + "\n";

        for (String key : headers.keySet())
            output += key + ":" + headers.get(key) + "\n";

        output += "\n";

        output += body + "\n";

        output += '\u0000';

        return output;
    }

    public void setCommand(String command){
        this.command = command;
    }

    public void addHeader(String key, String val){
        headers.put(key, val);
    }

    public void setBody(String body){
        this.body = body;
    }

    public String getCommand(){
        return command;
    }

    public HashMap<String, String> getHeaders(){
        return headers;
    }

    public String getBody() {
        return body;
    }
}
