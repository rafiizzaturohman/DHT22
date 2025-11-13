<?php

namespace App\Http\Controllers;

use App\Models\Dht22;
use Illuminate\Http\Request;

class Dht22Controller extends Controller
{
    public function __construct() 
    {
        $dht = Dht22::count();
        if($dht == 0){
            Dht22::create([
                'temperature' => 0,
                'humidity' => 0
            ]);
        }
    }
    public function updateData($tmp, $hmd){
        $dht = Dht22::first();
        $dht->temperature = $tmp;
        $dht->humidity = $hmd;
        $dht->save();

        return response()->json(['message' => 'Data updated successfully']);
    }

    public function getData(){
        $dht = Dht22::first();
        return response()->json($dht);
    }
}
