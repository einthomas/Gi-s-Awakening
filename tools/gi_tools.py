bl_info = {
    "name": "Gi Level-Editing Tools",
    "author": "Felix Kugler",
    "version": (0, 5),
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
from math import sqrt, ceil
import subprocess
import sys
    
def write_object(object, offset, vbo):
    matrix = object.matrix_world;
    mesh = object.to_mesh(bpy.context.scene, True, 'RENDER')
        
    vertices = mesh.vertices
    
    texture = mesh.tessface_uv_textures["Texture"]
    
    for face_index in range(len(mesh.tessfaces)):
        face = mesh.tessfaces[face_index]
        
        texture_face = texture.data[face_index]
        #texture_path = texture.image
        texture_uvs = texture_face.uv
        
        vertex_indices = range(3)
        if len(face.vertices) == 4:
            vertex_indices = [0, 1, 2, 2, 3, 0]
        
        if len(face.vertices) in [3, 4]:
            for i in vertex_indices:
                vbo.write(
                    struct.pack(
                        'fff', 
                        *(matrix * vertices[face.vertices[i]].co - offset)
                    ) + 
                    struct.pack('ff', *texture_uvs[i]) +
                    struct.pack(
                        'fff', *(matrix * Vector(
                            vertices[face.vertices[i]].normal[:] + (0,)
                        ))[:3]
                    )
                )
        else:
            print("Polygon is neither a triangle nor a square. Skipped.")
    
def write_group(group, directory): 
    offset = group.dupli_offset
    properties = {"name": group.name}
    with open(path.join(directory, group.name + ".vbo"), "wb") as vbo:
        for object in group.objects:
            try:
                if object.type == 'MESH':
                    write_object(object, offset, vbo)
                    
                    try: properties["size"] = object["size"][:]
                    except KeyError: pass
                    
                    try: properties["colorTexture"] = object["colorTexture"]
                    except KeyError: pass
                    
                    try: properties["linesTexture"] = object["linesTexture"]
                    except KeyError: pass
                    
            except Exception as e:
                print("Couldn't convert object {} ({}). Skipped.".format(
                    object.name, e
                ))
            
    return properties

def write_gi_block(context, filepath):
    directory, filename = path.split(filepath)
    
    blocks = []
    
    for group in bpy.data.groups:
        if group.name != "Start" and len(group.objects) > 0:
            blocks += [write_group(group, directory)]
        
    with open(filepath, 'w', encoding='utf-8') as f:
        json.dump(blocks, f, indent=4)

    return {'FINISHED'}
            
def write_gi_level(context, filepath, level_name):
    print("running write_gi_level...")
    
    platforms = []
    triggers = []
    pressure_plates = []
    start = [0, 0, 0]
    start_orientation = 0
    end = [0, 0, 0]
    end_lightmap_index = 0
    
    lightmap = bpy.data.images.get("lightmap")
    assert lightmap is not None
        
        
    lightmap_object_count = 0
    for object in bpy.context.scene.objects:
        if object.dupli_group is not None:
            lightmap_object_count += 1
        
    lightmap_grid = ceil(sqrt(lightmap_object_count))
    
    lightmap_index = 0
    
    for object in bpy.context.scene.objects:
        #try:
        if object.dupli_group is not None:
            type = object.dupli_group.name
                
            lightmap_index += 1
            
            if type == "Start":
                start = object.location[:]
                start_orientation = object.rotation_euler[2]
            elif type == "End":
                end = object.location[:]
                end_lightmap_index = lightmap_index
            elif type == "TriggerDiamond":
                properties = {
                    "position": object.location[:],
                    "type": type,
                    "isTriggered": object.get("isTriggered"),
                    "triggers": object.get("triggers"),
                    "lightMapIndex": lightmap_index
                }
                
                try: properties["movement"] = object["movement"][:]
                except KeyError: pass
                
                triggers += [properties]
            elif type == "PressurePlate":
                pressure_plates += [{
                    "position": object.location[:],
                    "type": type,
                    "givesAbility": object.get("givesAbility"),
                    "lightMapIndex": lightmap_index
                }]
            else:
                properties = {
                    "position": object.location[:],
                    "type": type,
                    "name": object.name,
                    "lightMapIndex": lightmap_index
                }
                
                try: properties["movement"] = object["movement"][:]
                except KeyError: pass
            
                platforms += [properties]
        
    #run Blender in background:
    # -b
    #run Python script:
    # -P <script>
        
    lightmap_path = filepath.rsplit('.', 1)[0] + '_lightmap.png'
    
    # Warning: only works if levels are saved in level folder 
    # and script is in tools folder
    directory = path.split(filepath)[0]
    
    script = path.join(directory, "..", "tools", "gi_tools.py")
    
    blender = subprocess.Popen([
        bpy.app.binary_path, bpy.data.filepath, "-b", "-P", script, "--", 
        "--bake", lightmap_path
    ])
            
    level = {
        "name": level_name,
        "platforms": platforms,
        "triggers": triggers,
        "pressurePlates": pressure_plates,
        "start": start,
        "startOrientation": start_orientation,
        "end": end,
        "lightMapSize": lightmap_grid,
        "lightMapPath": path.basename(lightmap_path),
        "endLightMapIndex": end_lightmap_index
    }
    
    with open(filepath, 'w', encoding='utf-8') as f:
        json.dump(level, f, indent=4)

    return {'FINISHED'}


def bake_block(object, lightmap, grid, index):
    """prepares a block for baking"""
    print("baking block...")
    factor = 1 / grid
    
    y = (index // grid) * factor
    x = (index % grid) * factor
    
    object.hide_render = True
    object.hide = True

    for sub_object in object.dupli_group.objects:
        if len(sub_object.material_slots) > 0:
            mesh_copy = sub_object.data.copy()
            
            for uv_face in mesh_copy.uv_textures["Texture"].data:
                uv_face.image = lightmap
                
            for vertex in mesh_copy.uv_layers["Texture"].data:
                vertex.uv = vertex.uv * factor + Vector((x, y))
                
            mesh_copy.update()
                
            bake_object = sub_object.copy()
            bake_object.data = mesh_copy
            
            bpy.context.scene.objects.link(bake_object)
            bpy.context.scene.update()
            
            bake_object.hide_render = False
            bake_object.hide = False
            
            bake_object.location += \
                object.location - object.dupli_group.dupli_offset
            
            bake_object.select = True
            bpy.context.scene.objects.active = bake_object
            
            mesh_copy.update()
            bpy.context.scene.update()
        
def bake(lightmap_path):
    bpy.context.scene.cycles.bake_type = 'DIFFUSE'
    bpy.context.scene.render.bake.use_pass_direct = True
    bpy.context.scene.render.bake.use_pass_indirect = True
    bpy.context.scene.render.bake.use_pass_color = False
    
    lightmap = bpy.data.images.get("lightmap")
    assert lightmap is not None
        
    lightmap_object_count = 0
    for object in bpy.context.scene.objects:
        if object.dupli_group is not None:
            lightmap_object_count += 1
        
    lightmap_grid = ceil(sqrt(lightmap_object_count))
    
    lightmap_index = 0

    bpy.ops.object.select_all(action='DESELECT')
        
    for object in bpy.context.scene.objects:
        if object.dupli_group is not None:
            type = object.dupli_group.name
            
            lightmap_index += 1
            
            bake_block(object, lightmap, lightmap_grid, lightmap_index)
                
    bpy.ops.object.bake(
        type = "DIFFUSE", 
        pass_filter = {"DIRECT", "INDIRECT"}, 
        width = 4096, height = 4096
    )
    
    lightmap.file_format = 'PNG'
    lightmap.filepath_raw = lightmap_path
    lightmap.save()
    
# ExportHelper is a helper class, defines filename and
# invoke() function which calls the file selector.
from bpy_extras.io_utils import ExportHelper
from bpy.props import StringProperty
from bpy.types import Operator


class ExportGiLevel(Operator, ExportHelper):
    """Export to Gi Level file"""
    # important since its how bpy.ops.import_test.some_data is constructed
    bl_idname = "gi.level_export"  
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
    # important since its how bpy.ops.import_test.some_data is constructed
    bl_idname = "gi.block_export"  
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
    print(sys.argv)
    try:
        arguments = sys.argv[sys.argv.index("--") + 1:]
        lightmap_path = arguments[arguments.index("--bake") + 1]
        bake(lightmap_path)
    
    except ValueError:
        try:
            unregister()
        except:
            pass
        register()
