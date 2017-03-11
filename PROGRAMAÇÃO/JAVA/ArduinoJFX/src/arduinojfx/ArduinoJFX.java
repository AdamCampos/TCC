/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package arduinojfx;

import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.image.Image;
import javafx.stage.Stage;

/**
 *
 * @author BUILD
 */
public class ArduinoJFX extends Application {

    @Override
    public void start(Stage stage) throws Exception {
        Parent root = FXMLLoader.load(getClass().getResource("FXMLDocument.fxml"));
        Scene scene = new Scene(root);
        scene.getStylesheets().add("/arduinojfx/css/principal.css");
        stage.setTitle("UNIFOA - TCC ELEVADOR");
        stage.setScene(scene);
        stage.getIcons().add(new Image("/arduinojfx/imagens/icone.png"));
        stage.show();
    }

    public static void main(String[] args) {
        launch(args);

    }

}
