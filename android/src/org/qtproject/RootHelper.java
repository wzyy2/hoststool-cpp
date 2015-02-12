package org.qtproject;

import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.List;
import java.util.Locale;
import android.util.Log;


public class RootHelper
{
    // this method will be called from C/C++
     public static int fibonacci(int n)
    {
        try{

            Process p = Runtime.getRuntime().exec("su");


        } catch (IOException e){
            e.printStackTrace();
        };
        return n;
    }
}
