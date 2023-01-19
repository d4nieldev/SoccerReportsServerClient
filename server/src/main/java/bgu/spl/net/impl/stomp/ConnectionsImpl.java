package bgu.spl.net.impl.stomp;

import java.io.IOException;
import java.sql.Connection;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.concurrent.ConcurrentHashMap;

import javax.swing.event.ListDataEvent;

import bgu.spl.net.srv.ConnectionHandler;
import bgu.spl.net.srv.Connections;

public class ConnectionsImpl<T> implements Connections<T> {
    // maps between connection id to the connection handler
    private ConcurrentHashMap<Integer, ConnectionHandler<T>> activeClients;

    public ConnectionsImpl(){
        this.activeClients = new ConcurrentHashMap<>();
    }

    @Override
    public void addClient(int connectionId, ConnectionHandler<T> handler){
        activeClients.put(connectionId, handler);
    }

    @Override
    public void send(int connectionId, T msg) throws IOException {
        activeClients.get(connectionId).send(msg);
    }

    @Override
    public void send(String channel, T msg) throws IOException {
        for (Integer connectionId : Data.getInstance().getSubscribers(channel))
            send(connectionId, msg);
    }

    @Override
    public void disconnect(int connectionId) {
        // remove from active clients
        activeClients.remove(connectionId);
    }
    
}
