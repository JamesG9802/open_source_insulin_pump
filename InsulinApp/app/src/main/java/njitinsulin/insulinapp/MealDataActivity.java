package njitinsulin.insulinapp;

import android.os.Bundle;
import android.view.View;

import androidx.appcompat.app.AppCompatActivity;

public class MealDataActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_meal_data);
    }

    public void toMainActivity(View view)   {
        setContentView(R.layout.activity_main);
    }
}