package arduinojfx;

import java.io.IOException;
import java.net.URISyntaxException;
import java.net.URL;
import java.util.Observable;
import java.util.Observer;
import java.util.ResourceBundle;
import java.util.logging.Level;
import java.util.logging.Logger;
import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.control.Label;
import javafx.scene.media.Media;
import javafx.scene.media.MediaPlayer;
import javafx.scene.shape.Circle;

public class FXMLDocumentController implements Initializable, Observer {

    private final SerialTest main = new SerialTest();

    @FXML
    private Circle btDesce;

    @FXML
    private Label numeroDisplay;

    @FXML
    private void handleBtSobe() {
        String str = "0jsobe";
        this.enviaDadosArduino(str);
        this.tocaSom("sobe");
        System.out.println("Enviando click sobe");
    }

    @FXML
    private void handleBtDesce() {
        String str = "0jdesce";
        this.enviaDadosArduino(str);
        this.tocaSom("desce");
        System.out.println("Enviando click desce");
    }

    @FXML
    private void handleBtAbre() {
        String str = "0jabre";
        this.enviaDadosArduino(str);
        this.tocaSom("abre");
        System.out.println("Enviando click abre");
    }

    @FXML
    private void handleBtFecha() {
        String str = "0jfecha";
        this.enviaDadosArduino(str);
        this.tocaSom("fecha");
        System.out.println("Enviando click fecha");
    }

    @FXML
    private void handleBtVentilacao() {
        String str = "0jvento";
        this.enviaDadosArduino(str);
        System.out.println("Enviando click ventilacao");
    }

    @FXML
    private void handleBtIluminacao() {
        String str = "0jluz";
        this.enviaDadosArduino(str);
        System.out.println("Enviando click iluminacao");
    }

    @FXML
    private void handleBtEmergencia() {
        String str = "0jparar";
        this.enviaDadosArduino(str);
        this.numeroDisplay.setText("E");
        System.out.println("Enviando click emergencia");
    }

    private void iniciaThreadSerial() {

        try {

            System.out.println("Iniciando FXML");
            this.main.addObserver(this);
            this.numeroDisplay.setText("0");
            this.main.initialize();

        } catch (Exception e) {
            System.out.println("Erro na aquisição do main");
        }

        try {
        } catch (Exception e) {
            //System.out.println("Erro na leitura main.getInput()");
        }

        Thread t = new Thread() {
            @Override
            public void run() {
                try {
                    Thread.sleep(1000);
                } catch (InterruptedException ie) {
                }
            }
        };

    }

    @Override
    public void initialize(URL url, ResourceBundle rb) {
        this.iniciaThreadSerial();
    }

    @Override
    public void update(Observable o, Object arg) {
        if (o.getClass().getSimpleName().equals("SerialTest")) {
            //Listener dos eventos de chegada
            String entrada = this.main.valorEntrada;
            System.out.println("mudança " + entrada);
            this.numeroDisplay.setText("entrada");
            System.out.println("Recebendo mudança " + entrada);

        }

    }

    private void enviaDadosArduino(String str) {
        try {
            this.main.serialPort.getOutputStream().write(str.getBytes());
        } catch (IOException ex) {
            Logger.getLogger(FXMLDocumentController.class.getName()).log(Level.SEVERE, null, ex);
        }
        try {
            this.main.serialPort.getOutputStream().flush();
        } catch (IOException ex) {
            Logger.getLogger(FXMLDocumentController.class.getName()).log(Level.SEVERE, null, ex);
        }
    }

    private void tocaSom(String som) {
        System.out.println(som.concat(".mp3"));
        String base = "/arduinojfx/sons/";
        String mp3 = base.concat(som).concat(".mp3");
        String path = null;
        try {
            path = this.getClass().getResource(mp3).toURI().toString();
        } catch (URISyntaxException ex) {
            Logger.getLogger(FXMLDocumentController.class.getName()).log(Level.SEVERE, null, ex);
        }
        Media media = new Media(path);
        MediaPlayer mediaPlayer = new MediaPlayer(media);
        mediaPlayer.play();
    }

}
