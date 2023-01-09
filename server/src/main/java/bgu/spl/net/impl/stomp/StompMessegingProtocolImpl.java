package bgu.spl.net.impl.stomp;

import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.srv.Connections;

public class StompMessegingProtocolImpl<T> implements StompMessagingProtocol<T> {

    @Override
    public void start(int connectionId, Connections<T> connections) {
        // TODO Auto-generated method stub
        
    }

    @Override
    public void process(T message) {
        // TODO Auto-generated method stub
        
    }

    @Override
    public boolean shouldTerminate() {
        // TODO Auto-generated method stub
        return false;
    }
}
