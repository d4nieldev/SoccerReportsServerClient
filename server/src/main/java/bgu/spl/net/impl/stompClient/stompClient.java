package bgu.spl.net.impl.stompClient;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.net.Socket;

public class stompClient {

    public static void main(String[] args) throws IOException {

        //BufferedReader and BufferedWriter automatically using UTF-8 encoding
        try (Socket sock = new Socket("localhost", 7777);
                BufferedReader in = new BufferedReader(new InputStreamReader(sock.getInputStream()));
                BufferedWriter out = new BufferedWriter(new OutputStreamWriter(sock.getOutputStream()))) {

            String message = "CONNECT\naccept-version:1.2\nhost:stomp.cs.bgu.ac.il\nlogin:meni\npasscode:films\n\n\u0000";
            System.out.println("sending");
            System.out.println(message);
            out.write(message);
            out.flush();

            System.out.println("received");
            String line;
            while ((line = in.readLine()) != null)
                    System.out.println(line);
        }
    }
}
