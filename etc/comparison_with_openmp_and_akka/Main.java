package newtemplet;

import akka.actor.ActorRef;
import akka.actor.ActorSystem;
import akka.actor.Props;
import newtemplet.akka.Actor;

import java.util.Random;


public class Main {

    private static final int SCALE = 400;

    private static final int W = SCALE * 2;
    private static final int H = SCALE;
    public static final int T = SCALE * 2;
    public static final int N = H - 2;

    private static double[][] field = new double[H][W];
    private static double[][] field1 = new double[H][W];

    private static final int OBS_N = 19;

    public static ActorSystem system;

    private static double obs_seq[] = new double[OBS_N];
    private static double seq_max, seq_mid, seq_min;

    private static double obs_akka[] = new double[OBS_N];
    private static double akka_max, akka_mid, akka_min;

    public static ActorRef actors[] = new ActorRef[N];
    public static int time[] = new int[N];


    public static void shufle() {
        Random random = new Random();
        for (int i = 0; i < H; i++) {
            for (int j = 0; j < W; j++) {
                double srand = random.nextDouble();
                field[i][j] = srand;
                field1[i][j] = srand;
            }
        }
    }

    private static double seq_alg() {
        long time = System.nanoTime();

        for (int t = 1; t <= T; t++)
            for (int i = 1; i < H - 1; i++) operation1(i);
        return ((double) System.nanoTime() - time) / 1E9;
    }

    private static boolean compare() {
        for (int i = 0; i < H; i++)
            for (int j = 0; j < W; j++)
                if (field1[i][j] != field[i][j]) {
                    System.out.println("\nWrong: i=" + i + ";j=" + j + "  seq=" + field1[i][j] + "  akka=" + field[i][j] + "\n");
                    return false;
                }
        return true;
    }

    private static void min_max() {
        seq_max = seq_min = obs_seq[0];
        akka_max = akka_min = obs_akka[0];

        for (int i = 1; i < OBS_N; i++) {
            seq_max = obs_seq[i] > seq_max ? obs_seq[i] : seq_max;
            seq_min = obs_seq[i] < seq_min ? obs_seq[i] : seq_min;

            akka_max = obs_akka[i] > akka_max ? obs_akka[i] : akka_max;
            akka_min = obs_akka[i] < akka_min ? obs_akka[i] : akka_min;
        }
        seq_mid = (seq_min + seq_max) / 2;
        akka_mid = (akka_min + akka_max) / 2;
    }

    public static void operation1(int i) {
        for (int j = 1; j < W - 1; j++) {
            field1[i][j] = (field1[i][j - 1] + field1[i][j + 1] + field1[i - 1][j] + field1[i + 1][j]) * 0.25;
        }
    }

    public static void operation(int i) {
        for (int j = 1; j < W - 1; j++) {
            field[i][j] = (field[i][j - 1] + field[i][j + 1] + field[i - 1][j] + field[i + 1][j]) * 0.25;
        }
    }

    static double par_akka() {
        system = ActorSystem.create("Akka");

        for (int i = 0; i < N; i++) {
            actors[i] = system.actorOf(Props.create(Actor.class, i));
            time[i] = 1;
        }

        long time = System.nanoTime();
        actors[0].tell(0, actors[0]);
        system.awaitTermination();
        return ((double) System.nanoTime() - time) / 1E9;
    }

    public static void main(String[] args) {
        for (int i = 0; i < OBS_N; i++) {
            shufle();
            obs_seq[i] = seq_alg();
            obs_akka[i] = par_akka();
            System.out.println(compare() ? "Akka Ok" : " something wrong in Akka");
            System.out.println((int) ((float) (i + 1) / OBS_N * 100) + "% done");
        }

        min_max();

        System.out.println("\nTest results for H = " + H + "; W = " + W + "; T = " + T);
        System.out.println("\nseq_min  = " + seq_min + " sec; " + "\nseq_mid  = " + seq_mid + " sec; \nseq_max  = " + seq_max + " sec\n");
        System.out.println("\nakka_min  = " + akka_min + " sec; " + "\nakka_mid  = " + akka_mid + " sec; \nakka_max  = " + akka_max + " sec\n");
    }


}