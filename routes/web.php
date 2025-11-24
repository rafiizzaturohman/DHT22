<?php

use App\Http\Controllers\Dht22Controller;
use Illuminate\Support\Facades\Route;

Route::get('/', function () {
    return view('welcome');
});

Route::get('/update-data/{tmp}/{hmd}', [Dht22Controller::class, 'updateData']);
Route::get('/get-data', [Dht22Controller::class, 'getData']);
Route::post('/update-nmax', [Dht22Controller::class, 'updateNilaiMaksimal']);
Route::post('/update-nmin', [Dht22Controller::class, 'updateNilaiMinimal']);