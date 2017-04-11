bl_info = {
    "name": "Gi Level-Editing Tools",
    "author": "Felix Kugler",
    "version": (0, 2),
    "blender": (2, 75, 0),
    "location": "File > Export",
    "description": "Provides Level Editing Tools for Gi's Awekening",
    "warning": "",
    "wiki_url": "",
    "category": "Export",
}

import bpy
import json
from os import path
import struct
from mathutils import Vector
    
def write_object(object, offset, vbo):
    matrix = object.matrix_world;
    mesh = object.to_mesh(bpy.context.scene, True, 'RENDER')
        
    vertices = mesh.vertices
    
    texture = mesh.tessface_uv_textures["Texture"]
    light = mesh.tessface_uv_textures["Light"]
    
    for face_index in range(len(mesh.tessfaces)):
        face = mesh.tessfaces[face_index]
        
        texture_face = texture.data[face_index]
        #texture_path = texture.image
        texture_uvs = texture_face.uv
        
        light_face = light.data[face_index]
        light_uvs = light_face.uv
        
        vertex_indices = range(3)
        if len(face.vertices) == 4:
            vertex_indices = [0, 1, 2, 2, 3, 0]
        
        if len(face.vertices) in [3, 4]:
            for i in vertex_indices:
                vbo.write(
                    struct.pack('fff', *(matrix * vertices[face.vertices[i]].co - offset)) + 
                    struct.pack('ff', *texture_uvs[i]) +
                    struct.pack('fff', *(matrix * Vector(vertices[face.vertices[i]].normal[:] + (0,)))[:3])
                )
        else:
            print("Polygon is neither a triangle nor a square. Skipped.")
    
def write_group(group, directory): 
    offset = group.dupli_offset
    size = [0, 0, 0]
    with open(path.join(directory, group.name + ".vbo"), "wb") as vbo:
        for object in group.objects:
            try:
                if object.type == 'MESH':
                    write_object(object, offset, vbo)
                    
                    size_property = object.get("size")
                    if size_property is not None:
                        size = size_property[:]
                    
            except Exception as e:
                print("Couldn't convert object {} ({}). Skipped.".format(object.name, e))
            
    return {"size": size, "name": group.name}

def write_gi_block(context, filepath):
    directory, filename = path.split(filepath)
    
    blocks = []
    
    for group in bpy.data.groups:
        if group.name not in ["Start", "End"]:
            blocks += [write_group(group, directory)]
        
    with open(filepath, 'w', encoding='utf-8') as f:
        json.dump(blocks, f, indent=4)

    return {'FINISHED'}

def write_gi_level(context, filepath, level_name):
    print("running write_gi_level...")
    
    platforms = []
    start = [0, 0, 0]
    start_orientation = 0
    end = [0, 0, 0]

    for object in bpy.context.scene.objects:
        try:
            type = object.dupli_group.name
            
            if type == "Start":
                start = object.location[:]
                start_orientation = object.rotation_euler[2]
            elif type == "End":
                end = object.location[:]
            else:
                platforms += [{
                    "position": object.location[:],
                    "type": type
                }]
                
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
        self.layout.operator(bl_idname, text="Gi Level (.gil)")
        
class ExportGiBlock(Operator, ExportHelper):
    """Export to Gi Block file"""
    bl_idname = "gi.block_export"  # important since its how bpy.ops.import_test.some_data is constructed
    bl_label = "Export Gi Block"

    # ExportHelper mixin class uses this
    filename_ext = ".gib"

    filter_glob = StringProperty(
        default="*.gib",
        options={'HIDDEN'},
        maxlen=255,  # Max internal buffer length, longer would be clamped.
    )

    def execute(self, context):
        return write_gi_block(context, self.filepath)

        
    def menu_func_export(self, context):
        self.layout.operator(bl_idname, text="Gi Block (.gib)")


def register():
    bpy.utils.register_class(ExportGiLevel)
    bpy.types.INFO_MT_file_export.append(ExportGiLevel.menu_func_export)
    bpy.utils.register_class(ExportGiBlock)
    bpy.types.INFO_MT_file_export.append(ExportGiBlock.menu_func_export)


def unregister():
    bpy.utils.unregister_class(ExportGiLevel)
    bpy.types.INFO_MT_file_export.remove(ExportGiLevel.menu_func_export)
    bpy.utils.unregister_class(ExportGiBlock)
    bpy.types.INFO_MT_file_export.remove(ExportGiBlock.menu_func_export)


if __name__ == "__main__":
    try:
        unregister()
    except:
        pass
    register()
