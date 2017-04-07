bl_info = {
    "name": "Gi Level-Editing Tools",
    "author": "Felix Kugler",
    "version": (0, 1),
    "blender": (2, 75, 0),
    "location": "File > Export > Gi Level (.gil)",
    "description": "Provides Level Editing Tools for Gi's Awekening",
    "warning": "",
    "wiki_url": "",
    "category": "Export",
}

import bpy
import json

def write_vertex(vertex, offset):
	pass
	
def write_object(object, offset, vbo):
	matrix = object.matrix_world;
    mesh = object.to_mesh(context.scene, True, 'RENDER')
    for faceIndex in range(len(mesh.tessfaces)):
        # TODO: read uv maps from mesh.tessface_uv_textures.keys()
        
        texture = mesh.tessface_uv_textures.active.data[faceIndex]
        image = texture.image
        
        if image != None:
            imageName = image.name
            face = mesh.tessfaces[faceIndex]
            vertices = mesh.vertices
            uvs = texture.uv
            
            if imageName not in images:
                images[imageName] = image
                triangles[imageName] = []
                distances[imageName] = 0
                count[imageName] = 0
            
            distance = (matrix * face.center).length
            distances[imageName] += distance
            count[imageName] += 1
            if len(face.vertices) == 3:
                for i in range(3):
                    vertexIndex = face.vertices[i]
                    triangles[imageName] += [(
                        struct.pack('fff', *(matrix * vertices[vertexIndex].co)) + struct.pack('ff', *uvs[i]), 
                        distance
                    )]
            elif len(face.vertices) == 4:
                for i in [0, 1, 2, 2, 3, 0]:
                    vertexIndex = face.vertices[i]
                    triangles[imageName] += [(
                        struct.pack('fff', *(matrix * vertices[vertexIndex].co)) + struct.pack('ff', *uvs[i]), 
                        distance
                    )]
            else:
                print("Polygon is neither a triangle nor a square. Skipped.")
	
def write_group(group, directory): 
	offset = group.dupli_offset
	with open(path.join(directory, group.name + ".vbo"), "wb") as vbo:
		for object in group.objects:
			try:
				if object.type == 'MESH':
			        write_object(object, offset, vbo)
			except:
				print("Couldn't convert object {}. Skipped.".format(object.name))

def write_gi_block(context, filepath, file_name):
    directory = path.split(self.filepath)[0]
    
	for group in bpy.data.groups:
		write_group(group, directory)

	return {'FINISHED'}

def write_gi_level(context, filepath, level_name):
    print("running write_gi_level...")
    
    platforms = []
    start = [0, 0, 0]
    start_orientation = 0
    end = [0, 0, 0]

    for object in bpy.context.scene.objects:
        try:
            inner_object = object.dupli_group.objects[0]
            size_property = inner_object.get("size")
            start_property = inner_object.get("start")
            end_property = inner_object.get("end")
            if size_property is not None:
                platforms += [{
                    "size": size_property[:],
                    "position": object.location[:],
                    "type": object.dupli_group.name
                }]
            elif start_property is not None:
                start = object.location[:]
                start_orientation = object.rotation_euler[2]
            elif end_property is not None:
                end = object.location[:]
                
        except Exception as e:
            print(e)
            
    level = {
        "name": level_name,
        "platforms": platforms,
        "start": start,
        "startOrientation": start_orientation,
        "end": end
    }
    
    with open(filepath, 'w', encoding='utf-8') as f:
        json.dump(level, f, indent=4)

    return {'FINISHED'}

# ExportHelper is a helper class, defines filename and
# invoke() function which calls the file selector.
from bpy_extras.io_utils import ExportHelper
from bpy.props import StringProperty
from bpy.types import Operator


class ExportGiLevel(Operator, ExportHelper):
    """Export to Gi Level file"""
    bl_idname = "gi.level_export"  # important since its how bpy.ops.import_test.some_data is constructed
    bl_label = "Export Gi Level"

    # ExportHelper mixin class uses this
    filename_ext = ".gil"

    filter_glob = StringProperty(
        default="*.gil",
        options={'HIDDEN'},
        maxlen=255,  # Max internal buffer length, longer would be clamped.
    )

    # List of operator properties, the attributes will be assigned
    # to the class instance from the operator settings before calling.
    level_name = StringProperty(
        name="Level name",
        description="The name that might show up in the level selection."
    )

    def execute(self, context):
        return write_gi_level(context, self.filepath, self.level_name)


def menu_func_export(self, context):
    self.layout.operator(ExportGiLevel.bl_idname, text="Gi Level (.gil)")


def register():
    bpy.utils.register_class(ExportGiLevel)
    bpy.types.INFO_MT_file_export.append(menu_func_export)


def unregister():
    bpy.utils.unregister_class(ExportGiLevel)
    bpy.types.INFO_MT_file_export.remove(menu_func_export)


if __name__ == "__main__":
    try:
        unregister()
    except:
        pass
    register()
