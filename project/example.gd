extends Node

func _ready() -> void:
	print("Ready")
	
	var stream1: AudioStreamOpus = load("res://samples/sample1.opus")
	print(stream1)
	print(stream1.tags)
	
	$AudioStreamPlayer.stream = stream1
	$AudioStreamPlayer.play()
