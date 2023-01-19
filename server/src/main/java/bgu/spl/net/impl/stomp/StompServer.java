package bgu.spl.net.impl.stomp;

import bgu.spl.net.srv.Server;

public class StompServer {

    public static void main(String[] args) {
        if(args.length < 2 ){
            //Not legal
            throw new IllegalArgumentException("Not legal arguments, Please insert port and server type.");
        }
        if(args[1].equals("reactor")){
            System.out.println("Reactor server chosen!");
            Server.stompThreadPerClient(
                    Short.valueOf(args[0]), //port
                    () -> new StompMessegingProtocolImpl(), //protocol factory
                    StompMessageEncoderDecoder::new //message encoder decoder factory
            ).serve();
        }

        else if(args[1].equals("tpc")){
            System.out.println("TPC server chosen!");
            Server.stompReactor(
                    Runtime.getRuntime().availableProcessors(),
                    Short.valueOf(args[0]), //port
                    () -> new StompMessegingProtocolImpl(), //protocol factory
                    StompMessageEncoderDecoder::new //message encoder decoder factory
            ).serve();
        }
        else{
            throw new IllegalArgumentException("Not legal server type, Please insert 'reactor' or 'tpc'.");
        }
    }
}
