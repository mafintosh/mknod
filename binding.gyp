{
  "targets": [
    {
      "target_name": "mknod",
      "sources": [ "mknod.cc" ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")",
      ],
    }
  ]
}
