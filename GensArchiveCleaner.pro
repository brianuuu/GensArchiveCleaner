QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    GlitterExternals/half/include/half.hpp \
    GlitterExternals/tinyxml2/tinyxml2.h \
    GlitterExternals/tristripper/detail/cache_simulator.h \
    GlitterExternals/tristripper/detail/connectivity_graph.h \
    GlitterExternals/tristripper/detail/graph_array.h \
    GlitterExternals/tristripper/detail/heap_array.h \
    GlitterExternals/tristripper/detail/policy.h \
    GlitterExternals/tristripper/detail/types.h \
    GlitterExternals/tristripper/public_types.h \
    GlitterExternals/tristripper/tri_stripper.h \
    GlitterLib/Animation.h \
    GlitterLib/BIXF.h \
    GlitterLib/BinaryReader.h \
    GlitterLib/BinaryWriter.h \
    GlitterLib/Bone.h \
    GlitterLib/Constants.h \
    GlitterLib/Emitter.h \
    GlitterLib/Endianness.h \
    GlitterLib/File.h \
    GlitterLib/GlitterEffect.h \
    GlitterLib/GlitterEnums.h \
    GlitterLib/GlitterMaterial.h \
    GlitterLib/Material.h \
    GlitterLib/MathGens.h \
    GlitterLib/Mesh.h \
    GlitterLib/Model.h \
    GlitterLib/Parameter.h \
    GlitterLib/Particle.h \
    GlitterLib/Submesh.h \
    GlitterLib/Texture.h \
    GlitterLib/Vertex.h \
    GlitterLib/VertexFormat.h \
    GlitterLib/xncpminumum.h \
    customfilesystemmodel.h \
    customtreeview.h \
    gensarchivecleaner.h

SOURCES += \
    GlitterExternals/tinyxml2/tinyxml2.cpp \
    GlitterExternals/tristripper/connectivity_graph.cpp \
    GlitterExternals/tristripper/policy.cpp \
    GlitterExternals/tristripper/tri_stripper.cpp \
    GlitterLib/Animation.cpp \
    GlitterLib/BIXF.cpp \
    GlitterLib/BinaryReader.cpp \
    GlitterLib/BinaryWriter.cpp \
    GlitterLib/Bone.cpp \
    GlitterLib/Emitter.cpp \
    GlitterLib/Endianness.cpp \
    GlitterLib/File.cpp \
    GlitterLib/GlitterEffect.cpp \
    GlitterLib/GlitterMaterial.cpp \
    GlitterLib/Material.cpp \
    GlitterLib/MathGens.cpp \
    GlitterLib/Mesh.cpp \
    GlitterLib/Model.cpp \
    GlitterLib/Parameter.cpp \
    GlitterLib/Particle.cpp \
    GlitterLib/Submesh.cpp \
    GlitterLib/Texture.cpp \
    GlitterLib/Vertex.cpp \
    GlitterLib/VertexFormat.cpp \
    GlitterLib/xncpminumum.cpp \
    customfilesystemmodel.cpp \
    customtreeview.cpp \
    gensarchivecleaner.cpp \
    main.cpp

FORMS += \
    gensarchivecleaner.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
