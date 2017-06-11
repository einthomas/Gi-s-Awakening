import bpy
import json
from os import path
import struct
from mathutils import Vector
from math import sqrt, ceil

# Gi bake script

def bake_block(object, lightmap, grid, index):
	"""prepares a block for baking"""
    print("baking block...")
    factor = 1 / grid
    
    y = (index // grid) * factor
    x = (index % grid) * factor
    
    object.hide_render = True
    object.hide = True
    
    # TODO: handle groups with multiple objects

    for sub_object in object.dupli_group.objects:
        bpy.ops.object.select_all(action='DESELECT')
        
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
            
            bake_object.location += 
				object.location - object.dupli_group.dupli_offset
            
            bake_object.select = True
            bpy.context.scene.objects.active = bake_object
            
            mesh_copy.update()
            bpy.context.scene.update()
		
bpy.context.scene.cycles.bake_type = 'DIFFUSE'
bpy.context.scene.render.bake.use_pass_direct = True
bpy.context.scene.render.bake.use_pass_indirect = True
bpy.context.scene.render.bake.use_pass_color = False

# TODO