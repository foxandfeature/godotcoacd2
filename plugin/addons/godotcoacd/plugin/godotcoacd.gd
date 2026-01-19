@tool
extends EditorPlugin

var shape_dialog: ConfirmationDialog
var shape_dialog_vbc: VBoxContainer
var shape_placement: OptionButton
var shape_type: OptionButton
var coacd_idx: int

var threads: Array[Thread] = []

func _enter_tree() -> void:
	CoACD.log_level = CoACD.LOG_INFO
	
	var mesh_editor := _find_mesh_editor()

	shape_dialog = _find_shape_dialog(mesh_editor)
	shape_dialog_vbc = shape_dialog.get_child(0)
	
	shape_placement = _find_option_button("Collision Shape Placement")
	shape_type = _find_option_button("Collision Shape Type")

	# Add custom option
	coacd_idx = shape_type.item_count
	shape_type.add_item("CoACD Multiple Convex")

	shape_dialog.confirmed.connect(confirm)

func _find_mesh_editor() -> Node:
	for child in EditorInterface.get_base_control().get_children():
		if child.get_class() == "MeshInstance3DEditor":
			return child
	return null

func _find_shape_dialog(mesh_editor: Node) -> ConfirmationDialog:
	for child in mesh_editor.get_children():
		if child is ConfirmationDialog and child.title == tr("Create Collision Shape"):
			return child
	return null

func _find_option_button(access_name: String) -> OptionButton:
	for child in shape_dialog_vbc.get_children():
		if child is OptionButton and child.accessibility_name == tr(access_name):
			return child
	return null

func confirm() -> void:
	if shape_type.selected != coacd_idx:
		return

	var selected_meshes := EditorInterface.get_selection().get_selected_nodes()
	for mesh_instance: MeshInstance3D in selected_meshes:
		var parent: Node = mesh_instance.get_parent()

		if shape_placement.selected == 1:
			parent = StaticBody3D.new()
			mesh_instance.add_child(parent, true)
			parent.owner = EditorInterface.get_edited_scene_root()

		var thread := Thread.new()
		threads.append(thread)
		
		var settings := load("res://addons/godotcoacd/editor_settings.tres")
		
		thread.start(
			decompose.bind(mesh_instance, settings, parent, EditorInterface.get_edited_scene_root(), thread),
			Thread.PRIORITY_HIGH
		)

func decompose(mesh_instance: MeshInstance3D, settings: CoACDSettings, parent: Node, scene_root: Node, thread: Thread) -> void:
	var concave := mesh_instance.mesh.create_trimesh_shape()
	var convexes: Array = CoACD.convex_decomposition(concave, settings)

	for shape in convexes:
		var collision := CollisionShape3D.new()
		collision.shape = shape
		call_deferred("add_shapes", collision, parent, scene_root)
	
	threads.erase(thread)

func add_shapes(shape: Node, parent: Node, root: Node) -> void:
	parent.add_child(shape, true)
	shape.owner = root

func _exit_tree() -> void:
	shape_type.remove_item(coacd_idx)
	shape_type.selected = 0
