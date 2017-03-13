package arduinojfx;

import gnu.io.CommPortIdentifier;
import gnu.io.NoSuchPortException;
import gnu.io.PortInUseException;
import gnu.io.SerialPort;
import gnu.io.SerialPortEvent;
import gnu.io.SerialPortEventListener;
import gnu.io.UnsupportedCommOperationException;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.TooManyListenersException;
import java.util.logging.Level;
import java.util.logging.Logger;
import javafx.beans.property.SimpleStringProperty;

public class SerialTest extends java.util.Observable implements SerialPortEventListener {

    //Declaração das variáveis da classe
    SerialPort serialPort;
    private BufferedReader input;
    private static final int DATA_RATE = 9600;
    public String valorEntrada = null;
    
    public SimpleStringProperty serialObservavel = new SimpleStringProperty();
    
    public void initialize() {
        CommPortIdentifier portId = null;
        
        try {
            //portId = CommPortIdentifier.getPortIdentifier("COM4");
            portId = CommPortIdentifier.getPortIdentifier("COM9");
        } catch (NoSuchPortException ex) {
            Logger.getLogger(SerialTest.class.getName()).log(Level.SEVERE, null, ex);
        }
        
        try {
            serialPort = (SerialPort) portId.open("JAVA", 2222);
            serialPort.setSerialPortParams(DATA_RATE, serialPort.DATABITS_8,
                    serialPort.STOPBITS_1, serialPort.PARITY_NONE);
            input = new BufferedReader(new InputStreamReader(serialPort.getInputStream()));
            serialPort.addEventListener(this);
            serialPort.notifyOnDataAvailable(true);
            
        } catch (PortInUseException | UnsupportedCommOperationException | IOException | TooManyListenersException e) {
            System.err.println("Erro 1 " + e.toString());
        }
        
    }
    
    public synchronized void close() {
        
        if (serialPort != null) {
            serialPort.removeEventListener();
            serialPort.close();
        }
        
    }
    
    @Override
    public synchronized void serialEvent(SerialPortEvent oEvent) {
        
        if (oEvent.getEventType() == SerialPortEvent.DATA_AVAILABLE) {
            Thread t = new Thread() {
                @Override
                public void run() {
                    try {
                        Thread.sleep(10000);
                        SerialTest.this.getInput();
                    } catch (InterruptedException ie) {
                    }
                }
            };
            t.start();
            
        }
        
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }
    
    public String getInput() {
        
        String retorno = null;
        
        if (input == null) {
            System.out.println("Erro no Input (null)");
        } else {
            try {
                retorno = input.readLine();
                
                if (retorno.contains("0u")) {
                    String retornoFiltrado = retorno.replace("0u", "");
                    //System.out.println("Entrada serial: " + retornoFiltrado);
                    this.valorEntrada = retornoFiltrado;
                    this.setChanged();
                    this.notifyObservers(this);

                }
            } catch (IOException ex) {
                //System.out.println("Erro grave no Input (null)");
                //retorno = "999";
            }
        }
        return retorno;
    }
    
}
