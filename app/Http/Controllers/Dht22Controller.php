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

        return response()->json([
            'message' => 'Data updated successfully',
            'data' => $dht
        ]);
    }

    public function getData(){
        $dht = Dht22::first();
        return response()->json($dht);
    }

    public function updateNilaiMaksimal(Request $request){
        //tambahkan validasi nilai
        $nilai = $request->nilai;
        $jenisNilai = $request->jenis_nilai;
        $dht = Dht22::first();
        
        if ($jenisNilai == 'max_temperature'){
            $dht->max_temperature = $nilai;
            $dht->save();
        } else if ($jenisNilai == 'max_humidity'){
            $dht->max_humidity = $nilai;
            $dht->save();
        } else {
            return response()->json([
                'error' => 'Gagal upload data',
            ]);
        }

        return redirect()->to('/');
    }
}
