package bgu.spl.net.impl.stomp;

import java.sql.Connection;
import java.util.HashMap;
import java.util.concurrent.ConcurrentHashMap;

import javax.lang.model.util.ElementScanner14;

import bgu.spl.net.srv.ConnectionHandler;
import bgu.spl.net.srv.Connections;

public class ConnectionsImpl<T> implements Connections<T> {
    private ConcurrentHashMap<Integer, ConnectionHandler<T>> activeClients;
    private ConcurrentHashMap<String, Boolean> loggedInUsers;
    private ConcurrentHashMap<Integer, Boolean> loggedInClients;
    private ConcurrentHashMap<String, String> users;

    public ConnectionsImpl(){
        this.activeClients = new ConcurrentHashMap<>();
        this.loggedInUsers = new ConcurrentHashMap<>();
        this.loggedInClients = new ConcurrentHashMap<>();
        this.users = new ConcurrentHashMap<>();
    }

    public void addClient(int connectionId, ConnectionHandler<T> handler){
        activeClients.put(connectionId, handler);
    }

    public synchronized String connect(int connectionId, String host, String login, String password) {
        if (activeClients.get(connectionId) != null){
            // the user is logged in from some client
            if (loggedInUsers.get(login))
                return "USER_ERR";
            // TODO maybe handle on client side
            // the client has some user logged in from it
            else if (loggedInClients.get(connectionId))
                return "CLIENT_ERR";
            // wrong password
            else if (users.computeIfAbsent(login, l -> password) != password)
                return "PASS_ERR";
            else
                return "SUCCESS";
        }
        return null;
    }

    @Override
    public boolean send(int connectionId, T msg) {
        // TODO Auto-generated method stub
        return false;
    }

    @Override
    public void send(String channel, T msg) {
        // TODO Auto-generated method stub
        
    }

    @Override
    public void disconnect(int connectionId) {
        // TODO Auto-generated method stub
        
    }
    
}
