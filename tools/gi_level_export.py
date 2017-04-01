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

def write_gi_level(context, filepath, level_name):
    print("running write_gi_level...")
    
    platforms = []
    start = [0, 0, 0]
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
                    "position": object.location[:]
                }]
            elif start_property is not None:
                start = object.location[:]
            elif end_property is not None:
                end = object.location[:]
                
        except Exception as e:
            print(e)
            
    level = {
        "name": level_name,
        "platforms": platforms,
        "start": start,
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
