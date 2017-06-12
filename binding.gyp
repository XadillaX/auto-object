{
  "targets": [{
    "target_name": "auto",
    "sources": [
      "src/auto.cc"
    ],
    "include_dirs": [
      "<!(node -e \"require('nan')\")"
    ]
  }]
}
