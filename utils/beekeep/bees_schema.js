{
    "sceneName": "drone"
    "moduleName": "aleph-waves.ldr"
    "moduleVersion": : { "maj": 0, "min": 0, "rev": 0 },
    "beesVersion": { "maj": 0, "min": 0, "rev": 0 },
    
    "ops": [
	{
	    "type":0, 
	    "name":"ENC", 
	    "ins": [
		{ "name": "MIN", "value": 12288, },
		{ "name": "MAX", "value": 18432 },
		{ "name": "STEP", "value": 64 },
	    ],
	    "outs": [
		{ "name": "VALUE", "target": { "type": "param", "index": 0, "name": "hz0" } }
	    ]
	}, {
	    "type":0, 
	    "name":"ENC", 
	    "ins": [
		{ "name": "MIN", "value": 0, },
		{ "name": "MAX", "value": 32767 },
		{ "name": "STEP", "value": 256 },
	    ]
	    "outs": [
		{ "name": "VALUE", "target": { "type": "param", "index": 1, "name": "hz1" } }
	    ]

	}, {
	    "type":1, 
	    "name":"SW", 
	    "ins": [
		{ "name": "MIN", "value": 0, },
		{ "name": "MAX", "value": 256 },
		{ "name": "STEP", "value": 256 },
	    ]
	    "outs": [
		{ "name": "VALUE", 
		  "target": { 
		    "type": "input", "opIdx": 0, "opName":"ENC", "opInIdx": 2, "opInName":"STEP"
		  } 
		}
	    ]
	}
    ],

    "params": [
	{ "name": "hz0", "unit": "hz", "min": 0.1, "max": 100000, "value": 220 },
	{ "name": "hz1", "unit": "hz", "min": 0.1, "max": 100000, "value": 222 },
	{ "name": "amp0", "unit": "amp", "min": 0.0, "max": 1.0, "value": 0.4 },
	{ "name": "amp1", "unit": "amp", "min": 0.0, "max": 1.0, "value": 0.4 }
    ],

    "presets": [
	{ 
	    "name":  "000_MINAMP",
	    "ins": [
		{ 
		    "type": "input", "opIdx": 2, "opName":"SW", "opInIdx": 0, "opInName":"MUL", "value":256
		}, {
		    "type": "param", "name": "amp0", "value": 0.5
		}
	    ],

	    "outs": [ 
		{ "opIdx": 2,
		  "opName": "SW", 
		  "outIdx": 0,
		  "outName": "VAL",
		  "target": { 
		      "type": "input", "opIdx": 1, "opName":"ENC", "opInIdx": 2, "opInName": "MIN" 
		  }
		}
	    ]
	}, { 
	    "name":  "001_tiny_knob",
	    "ins": [
		{
		    "type": "input", "opIdx": 0, "opName":"ENC", "opInIdx": 2, "opInName":"STEP", "value": 1 
		}
	    ]
	}
    ]
}
