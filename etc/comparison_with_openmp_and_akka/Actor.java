package newtemplet.akka;

import akka.actor.UntypedActor;
import newtemplet.Main;

import static newtemplet.Main.N;

public class Actor extends UntypedActor {

    protected int id;
    private boolean access_ms_id_minus_1 = false;
    private boolean access_ms_id = true;

    public Actor() {

    }

    public Actor(int id) {
        this.id = id;
    }

    @Override
    public void onReceive(Object message) {
        if (((Integer) message) == id - 1) access_ms_id_minus_1 = true;
        if (((Integer) message) == id) access_ms_id = true;
            
        if ((id == 0 || access_ms_id_minus_1) &&
                (id == N - 1 || access_ms_id) &&
                (Main.time[id] <= Main.T)) {
            Main.operation(id + 1);
            Main.time[id]++;

            if (id != 0) {
                Main.actors[id - 1].tell(id - 1, getSelf());
                access_ms_id_minus_1 = false;
            }
            if (id != Main.N - 1) {
                Main.actors[id + 1].tell(id, getSelf());
                access_ms_id = false;
            }
        }
        if (Main.time[id] == Main.T + 1 && id == Main.N - 1) {
            Main.system.terminate();
        }
    }
}