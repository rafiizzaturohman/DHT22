<?php

use Illuminate\Database\Migrations\Migration;
use Illuminate\Database\Schema\Blueprint;
use Illuminate\Support\Facades\Schema;

return new class extends Migration
{
    /**
     * Run the migrations.
     */
    public function up(): void
    {
        Schema::table('dht22s', function (Blueprint $table) {
            $table->float('max_temperature')->default(0)->after('temperature');
            $table->float('max_humidity')->default(0)->after('humidity');
        });
    }

    /**
     * Reverse the migrations.
     */
    public function down(): void
    {
        Schema::table('dht22s', function (Blueprint $table) {
            $table->dropColumn('max_temperature');
            $table->dropColumn('max_humidity');
        });
    }
};
